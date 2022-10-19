#include "fs.h"

#include "event.h"
#include "heap.h"
#include "queue.h"
#include "thread.h"

#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct fs_t
{
	heap_t* heap;
	queue_t* file_queue;
	thread_t* file_thread;

	queue_t* compress_queue;
	thread_t* compress_thread;
	
	queue_t* decompress_queue;
	thread_t* decompress_thread;

	// Acts as a flag to if we are using regular file, compression, or decompression queue
	// 0 - normal file, 1 - compress, 2 - decompress
	int which_queue;
} fs_t;

typedef enum fs_work_op_t
{
	k_fs_work_op_read,
	k_fs_work_op_write,
} fs_work_op_t;

typedef struct fs_work_t
{
	heap_t* heap;
	fs_work_op_t op;
	char path[1024];
	bool null_terminate;
	bool use_compression;
	void* buffer;
	size_t size;
	event_t* done;
	int result;
	int compress_size;
	char* buffer_2;
} fs_work_t;

static int file_thread_func(void* user);
static int compress_thread_func(void* user);
static int decompress_thread_func(void* user);

fs_t* fs_create(heap_t* heap, int queue_capacity)
{
	fs_t* fs = heap_alloc(heap, sizeof(fs_t), 8);
	fs->heap = heap;
	fs->file_queue = queue_create(heap, queue_capacity);
	fs->which_queue = 0;
	fs->file_thread = thread_create(file_thread_func, fs);

	// initialize compress / decompress queue and thread
	fs->compress_queue = queue_create(heap, queue_capacity);
	fs->which_queue = 1;
	fs->compress_thread = thread_create(compress_thread_func, fs);
	
	fs->decompress_queue = queue_create(heap, queue_capacity);
	fs->which_queue = 2;
	fs->decompress_thread = thread_create(decompress_thread_func, fs);
	return fs;
}

void fs_destroy(fs_t* fs)
{
	queue_push(fs->file_queue, NULL);
	thread_destroy(fs->file_thread);
	queue_destroy(fs->file_queue);

	queue_push(fs->compress_queue, NULL);
	thread_destroy(fs->compress_thread);
	queue_destroy(fs->compress_queue);
	
	queue_push(fs->decompress_queue, NULL);
	thread_destroy(fs->decompress_thread);
	queue_destroy(fs->decompress_queue);

	heap_free(fs->heap, fs);
}

fs_work_t* fs_read(fs_t* fs, const char* path, heap_t* heap, bool null_terminate, bool use_compression)
{
	fs_work_t* work = heap_alloc(fs->heap, sizeof(fs_work_t), 8);
	work->heap = heap;
	work->op = k_fs_work_op_read;
	strcpy_s(work->path, sizeof(work->path), path);
	work->buffer = NULL;
	work->size = 0;
	work->done = event_create();
	work->result = 0;
	work->null_terminate = null_terminate;
	work->use_compression = use_compression;

	if (work->use_compression)
	{
		queue_push(fs->decompress_queue, work);
	}
	else 
	{
		queue_push(fs->file_queue, work);
	}
	return work;
}

fs_work_t* fs_write(fs_t* fs, const char* path, const void* buffer, size_t size, bool use_compression)
{
	
	fs_work_t* work = heap_alloc(fs->heap, sizeof(fs_work_t), 8);
	work->heap = fs->heap;
	work->op = k_fs_work_op_write;
	strcpy_s(work->path, sizeof(work->path), path);
	work->buffer = (void*)buffer;
	work->size = size;
	work->done = event_create();
	work->result = 0;
	work->null_terminate = false;
	work->use_compression = use_compression;

	if (use_compression)
	{
		// HOMEWORK 2: Queue file write work on compression queue!

		// Create a temporary buffer and initialize the memory
		work->buffer_2 = heap_alloc(fs->heap, work->size, 8);
		for (int i = 0; i < work->size; i++) {
			work->buffer_2[i] = "";
		}

		// compress the string into newly created buffer and save the compression_size
		// LZ4_compress_default(const char* src, char* dst, int srcSize, int dstCapacity)
		work->compress_size = LZ4_compress_default(work->buffer, work->buffer_2, work->size, strlen(work->buffer_2));

		queue_push(fs->compress_queue, work);
	}
	else
	{
		queue_push(fs->file_queue, work);
	}

	return work;
}

bool fs_work_is_done(fs_work_t* work)
{
	return work ? event_is_raised(work->done) : true;
}

void fs_work_wait(fs_work_t* work)
{
	if (work)
	{
		event_wait(work->done);
	}
}

int fs_work_get_result(fs_work_t* work)
{
	fs_work_wait(work);
	return work ? work->result : -1;
}

void* fs_work_get_buffer(fs_work_t* work)
{
	fs_work_wait(work);
	return work ? work->buffer : NULL;
}

size_t fs_work_get_size(fs_work_t* work)
{
	fs_work_wait(work);
	return work ? work->size : 0;
}

void fs_work_destroy(fs_work_t* work)
{
	if (work)
	{
		event_wait(work->done);
		event_destroy(work->done);
		heap_free(work->heap, work);
	}
}

static void file_read(fs_work_t* work)
{
	wchar_t wide_path[1024];
	if (MultiByteToWideChar(CP_UTF8, 0, work->path, -1, wide_path, sizeof(wide_path)) <= 0)
	{
		work->result = -1;
		return;
	}

	HANDLE handle = CreateFile(wide_path, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		work->result = GetLastError();
		return;
	}

	if (!GetFileSizeEx(handle, (PLARGE_INTEGER)&work->size))
	{
		work->result = GetLastError();
		CloseHandle(handle);
		return;
	}

	work->buffer = heap_alloc(work->heap, work->null_terminate ? work->size + 1 : work->size, 8);

	DWORD bytes_read = 0;
	int temp_size = 0;

	if (work->use_compression)
	{
		// Read the compression_size
		if (!ReadFile(handle, &temp_size, (DWORD)4, &bytes_read, NULL))
		{
			work->result = GetLastError();
			CloseHandle(handle);
			return;
		}
		// Read the remaining buffer
		if (!ReadFile(handle, work->buffer, (DWORD) temp_size, &bytes_read, NULL))
		{
			work->result = GetLastError();
			CloseHandle(handle);
			return;
		}
	}
	else
	{
		if (!ReadFile(handle, work->buffer, (DWORD)work->size, &bytes_read, NULL))
		{
			work->result = GetLastError();
			CloseHandle(handle);
			return;
		}
	}
	
	work->size = bytes_read;
	if (work->null_terminate)
	{
		((char*)work->buffer)[bytes_read] = 0;
	}

	CloseHandle(handle);

	if (work->use_compression)
	{
		// HOMEWORK 2: Queue file read work on decompression queue!

		// Allocate a temporary buffer that will hold what we are going to decompress
		char* temp_buf = heap_alloc(work->heap, 1000000, 8);

		// LZ4_decompress_safe(const char* src, char* dst, int compressedSize, int dstCapacity);
		int decompress_size = LZ4_decompress_safe(work->buffer, temp_buf, temp_size, 1000000);
		work->size = decompress_size;
		temp_buf[decompress_size] = 0;

		heap_free(work->heap, work->buffer);

		// Reallocate the initial buffer, so that we can return this decompressed string in the original buffer
		work->buffer = heap_alloc(work->heap, decompress_size, 8);
		for (int i = 0; i < decompress_size; i++)
		{
			((char*)work->buffer)[i] = "";
		}

		strcpy_s(work->buffer, strlen(work->buffer), temp_buf);

		heap_free(work->heap, temp_buf);

		event_signal(work->done);
	}
	else
	{
		event_signal(work->done);
	}
}


int get_hash(void* address, int bucket_count)
{
	return (intptr_t)address % bucket_count;
}

static void file_write(fs_work_t* work)
{
	wchar_t wide_path[1024];
	if (MultiByteToWideChar(CP_UTF8, 0, work->path, -1, wide_path, sizeof(wide_path)) <= 0)
	{
		work->result = -1;
		return;
	}
	HANDLE handle = CreateFile(wide_path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		printf("HERE ERROR\n");
		work->result = GetLastError();
		return;
	}
	
	DWORD bytes_written = 0;

	if (work->use_compression)
	{
		// Write 4 bytes for the integer of the compression_size
		if (!WriteFile(handle, &(work->compress_size), (DWORD)4, &bytes_written, NULL))
		{
			work->result = GetLastError();
			CloseHandle(handle);
			return;
		}
		// Write the compression_size bytes from buffer_2
		// Free allocated memory from buffer_2 as well
		if (!WriteFile(handle, work->buffer_2, (DWORD)(work->compress_size), &bytes_written, NULL))
		{
			work->result = GetLastError();
			CloseHandle(handle);
			return;
		}
		work->size = strlen(work->buffer);
		heap_free(work->heap, work->buffer_2);
	}
	else
	{
		if (!WriteFile(handle, work->buffer, (DWORD)work->size, &bytes_written, NULL))
		{
			work->result = GetLastError();
			CloseHandle(handle);
			return;
		}
		work->size = bytes_written;

	}
	
	CloseHandle(handle);

	event_signal(work->done);
}

static int file_thread_func(void* user)
{
	fs_t* fs = user;
	while (true)
	{
		fs_work_t* work = queue_pop(fs->file_queue);

		if (work == NULL)
		{
			break;
		}

		switch (work->op)
		{
		case k_fs_work_op_read:
			file_read(work);
			break;
		case k_fs_work_op_write:
			file_write(work);
			break;
		}
	}
	return 0;
}

static int compress_thread_func(void* user)
{
	fs_t* fs = user;
	while (true)
	{
		fs_work_t* work = queue_pop(fs->compress_queue);

		if (work == NULL)
		{
			break;
		}

		switch (work->op)
		{
		case k_fs_work_op_read:
			file_read(work);
			break;
		case k_fs_work_op_write:
			file_write(work);
			break;
		}
	}
	return 0;
}

static int decompress_thread_func(void* user)
{
	fs_t* fs = user;
	while (true)
	{
		fs_work_t* work = queue_pop(fs->decompress_queue);

		if (work == NULL)
		{
			break;
		}

		switch (work->op)
		{
		case k_fs_work_op_read:
			file_read(work);
			break;
		case k_fs_work_op_write:
			file_write(work);
			break;
		}
	}
	return 0;
}




///////////////////////////////////////////////////////////////////////////////////////////////




//#include "fs.h"
//#include "lz4.h"
//
//#include "event.h"
//#include "heap.h"
//#include "queue.h"
//#include "thread.h"
//
//#include <string.h>
//
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//
//typedef struct fs_t
//{
//	heap_t* heap;
//	queue_t* file_queue;
//	thread_t* file_thread;
//
//	queue_t* compress_queue;
//	thread_t* compress_thread;
//
//	queue_t* decompress_queue;
//	thread_t* decompress_thread;
//} fs_t;
//
//typedef enum fs_work_op_t
//{
//	k_fs_work_op_read,
//	k_fs_work_op_write,
//} fs_work_op_t;
//
//typedef struct fs_work_t
//{
//	heap_t* heap;
//	fs_work_op_t op;
//	char path[1024];
//	bool null_terminate;
//	bool use_compression;
//	void* buffer;
//	size_t size;
//	event_t* done;
//	int result;
//} fs_work_t;
//
//static int file_thread_func(void* user);
//
//fs_t* fs_create(heap_t* heap, int queue_capacity)
//{
//	fs_t* fs = heap_alloc(heap, sizeof(fs_t), 8);
//	fs->heap = heap;
//	fs->file_queue = queue_create(heap, queue_capacity);
//	fs->file_thread = thread_create(file_thread_func, fs);
//
//	// initialize compress / decompress queue and thread
//	fs->compress_queue = queue_create(heap, queue_capacity);
//	fs->compress_thread = thread_create(file_thread_func, fs);
//	
//	fs->decompress_queue = queue_create(heap, queue_capacity);
//	fs->decompress_thread = thread_create(file_thread_func, fs);
//
//	return fs;
//}
//
//void fs_destroy(fs_t* fs)
//{
//	printf("HERE B 1\n");
//	queue_push(fs->file_queue, NULL);
//	printf("HERE B 1 1\n");
//	thread_destroy(fs->file_thread);
//	printf("HERE B 1 2\n");
//	queue_destroy(fs->file_queue);
//	printf("HERE B 1 3\n");
//
//	printf("HERE B 2\n");
//	queue_push(fs->compress_queue, NULL);
//	thread_destroy(fs->compress_thread);
//	queue_destroy(fs->compress_queue);
//
//	printf("HERE B 3\n");
//	queue_push(fs->decompress_queue, NULL);
//	thread_destroy(fs->decompress_thread);
//	queue_destroy(fs->decompress_queue);
//
//	printf("HERE B 4\n");
//	heap_free(fs->heap, fs);
//}
//
//fs_work_t* fs_read(fs_t* fs, const char* path, heap_t* heap, bool null_terminate, bool use_compression)
//{
//	fs_work_t* work = heap_alloc(fs->heap, sizeof(fs_work_t), 8);
//	work->heap = heap;
//	work->op = k_fs_work_op_read;
//	strcpy_s(work->path, sizeof(work->path), path);
//	work->buffer = NULL;
//	work->size = 0;
//	work->done = event_create();
//	work->result = 0;
//	work->null_terminate = null_terminate;
//	work->use_compression = use_compression;
//	if (work->use_compression)
//	{
//		queue_push(fs->decompress_queue, work);
//	}
//	else 
//	{
//		queue_push(fs->file_queue, work);
//	}
//	return work;
//}
//
//fs_work_t* fs_write(fs_t* fs, const char* path, const void* buffer, size_t size, bool use_compression)
//{
//	fs_work_t* work = heap_alloc(fs->heap, sizeof(fs_work_t), 8);
//	work->heap = fs->heap;
//	work->op = k_fs_work_op_write;
//	
//	work->buffer = (void*)buffer;
//	work->size = size;
//	work->done = event_create();
//	work->result = 0;
//	work->null_terminate = false;
//	work->use_compression = use_compression;
//
//
//	if (use_compression)
//	{
//		// HOMEWORK 2: Queue file write work on compression queue!
//		// LZ4_compress_default(const char* src, char* dst, int srcSize, int dstCapacity)
//		int ret = LZ4_compress_default(work->buffer, work->path, work->size, sizeof(work->path));
//		if (ret == 0)
//		{
//			//ERROR
//		}
//		queue_push(fs->compress_queue, work);
//	}
//	else
//	{
//		strcpy_s(work->path, sizeof(work->path), path);
//		queue_push(fs->file_queue, work);
//	}
//
//	return work;
//}
//
//bool fs_work_is_done(fs_work_t* work)
//{
//	return work ? event_is_raised(work->done) : true;
//}
//
//void fs_work_wait(fs_work_t* work)
//{
//	if (work)
//	{
//		event_wait(work->done);
//	}
//}
//
//int fs_work_get_result(fs_work_t* work)
//{
//	printf("HEREA1\n");
//	fs_work_wait(work);
//	return work ? work->result : -1;
//}
//
//void* fs_work_get_buffer(fs_work_t* work)
//{
//	fs_work_wait(work);
//	return work ? work->buffer : NULL;
//}
//
//size_t fs_work_get_size(fs_work_t* work)
//{
//	fs_work_wait(work);
//	return work ? work->size : 0;
//}
//
//void fs_work_destroy(fs_work_t* work)
//{
//	if (work)
//	{
//		event_wait(work->done);
//		event_destroy(work->done);
//		heap_free(work->heap, work);
//	}
//}
//
//static void file_read(fs_work_t* work)
//{
//	wchar_t wide_path[1024];
//	if (MultiByteToWideChar(CP_UTF8, 0, work->path, -1, wide_path, sizeof(wide_path)) <= 0)
//	{
//		work->result = -1;
//		return;
//	}
//
//	HANDLE handle = CreateFile(wide_path, GENERIC_READ, FILE_SHARE_READ, NULL,
//		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (handle == INVALID_HANDLE_VALUE)
//	{
//		work->result = GetLastError();
//		return;
//	}
//
//	if (!GetFileSizeEx(handle, (PLARGE_INTEGER)&work->size))
//	{
//		work->result = GetLastError();
//		CloseHandle(handle);
//		return;
//	}
//
//	work->buffer = heap_alloc(work->heap, work->null_terminate ? work->size + 1 : work->size, 8);
//
//	DWORD bytes_read = 0;
//	if (!ReadFile(handle, work->buffer, (DWORD)work->size, &bytes_read, NULL))
//	{
//		work->result = GetLastError();
//		CloseHandle(handle);
//		return;
//	}
//
//	work->size = bytes_read;
//	if (work->null_terminate)
//	{
//		((char*)work->buffer)[bytes_read] = 0;
//	}
//
//	CloseHandle(handle);
//
//	if (work->use_compression)
//	{
//		// LZ4_decompress_safe(const char* src, char* dst, int compressedSize, int dstCapacity);
//		LZ4_decompress_safe(work->buffer, work->path, work->size, sizeof(work->path));
//		event_signal(work->done);
//		// HOMEWORK 2: Queue file read work on decompression queue!
//	}
//	else
//	{
//		event_signal(work->done);
//	}
//}
//
//static void file_write(fs_work_t* work)
//{
//	wchar_t wide_path[1024];
//	if (MultiByteToWideChar(CP_UTF8, 0, work->path, -1, wide_path, sizeof(wide_path)) <= 0)
//	{
//		work->result = -1;
//		return;
//	}
//
//	HANDLE handle = CreateFile(wide_path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
//		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (handle == INVALID_HANDLE_VALUE)
//	{
//		work->result = GetLastError();
//		return;
//	}
//
//	DWORD bytes_written = 0;
//	if (!WriteFile(handle, work->buffer, (DWORD)work->size, &bytes_written, NULL))
//	{
//		work->result = GetLastError();
//		CloseHandle(handle);
//		return;
//	}
//
//	work->size = bytes_written;
//
//	CloseHandle(handle);
//
//	event_signal(work->done);
//}
//
//static int file_thread_func(void* user)
//{
//	fs_t* fs = user;
//	while (true)
//	{
//		fs_work_t* work = queue_pop(fs->file_queue);			// maybe problem here?
//		if (work == NULL)
//		{
//			break;
//		}
//
//		switch (work->op)
//		{
//		case k_fs_work_op_read:
//			file_read(work);
//			break;
//		case k_fs_work_op_write:
//			file_write(work);
//			break;
//		}
//	}
//	return 0;
//}