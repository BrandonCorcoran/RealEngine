/** @file fs.h
*	@brief A header file for an asynchronous read/write file system
* 
*	This set of operations allows for reading and writing of files
*	in an asynchronous manner. There is also the option for the 
*	files to be compressed, which uses lz4.h.
*/
#pragma once

#include <stdbool.h>

/**	@brief A handle to a file system object
*
*	This will hold all relevant information that is needed for a file system.
*	The file system will be used to read
*	in information relevant to the game, such as .vert and .frag files, used to render geometry.
*/
typedef struct fs_t fs_t;

/**	@brief A handle to a file work object
*
*	This will hold all relevant information that is needed to do work
*	on a file. A buffer is contained in here, along with the option
*	for a compressed buffer
*/
typedef struct fs_work_t fs_work_t;

/** @brief A handle to a heap object
*
*	This will hold all relevant information that is needed for the heap.
*	More about the heap can be found in heap.h. The heap will be used
*	for memory allocation and deallocation.
*/
typedef struct heap_t heap_t;

/**	@brief A function to create a file system
*	@param heap a pointer to a heap_t object, which is how memory is managed
*	@param queue_capacity an integer, which is the maximum number of file operations
*	@returns an fs_t pointer, which is the new file system
* 
*	Create a new file system.
*	Provided heap will be used to allocate space for queue and work buffers.
*	Provided queue size defines number of in-flight file operations.
*	This file system will need to be destroyed before program termination
*	to ensure all allocated memory is properly deallocated.
*/
fs_t* fs_create(heap_t* heap, int queue_capacity);

/**	@brief A function to destroy a file system
*	@param fs_t a pointer to a fs_t object, which is the file system
*	@returns none
* 
*	Destroy a previously created file system. This is necessary
*	to ensure allocated memory is properly deallocated
*/
void fs_destroy(fs_t* fs_t);

/**	@brief A function to read a file
*	@param fs a pointer to a fs_t object, which is the file system
*	@param path a const char*, which is the path or filename to read from
*	@param heap a pointer to a heap_t object, which is how memory is managed
*	@param null_terminate a boolean, which determines if the user wants what is read in to be null terminating or not (true if desired)
*	@param use_compression a boolean, which determines if the user wants what is read to be compressed (true if desired)
*	@returns an fs_work_t pointer, which holds the buffer to what is read in
* 
*	Queue a file read.
*	File at the specified path will be read in full.
*	Memory for the file will be allocated out of the provided heap.
*	It is the calls responsibility to free the memory allocated.
*	Returns a work object.
*/
fs_work_t* fs_read(fs_t* fs, const char* path, heap_t* heap, bool null_terminate, bool use_compression);

/**	@brief A function to write to a file
*	@param fs a pointer to a fs_t object, which is the file system
*	@param path a const char*, which is the path or filename to write to
*	@param buffer a pointer to a const void object, which is what we want written to the file
*	@param size a size_t, which is the size of the passed in buffer
*	@param use_compression a boolean, which determines if the user wants what is read to be compressed (true if desired)
*	@returns an fs_work_t pointer, which holds the buffer to what is written
* 
*	Queue a file write.
*	File at the specified path will be written in full.
*	Returns a work object.
*/
fs_work_t* fs_write(fs_t* fs, const char* path, const void* buffer, size_t size, bool use_compression);

/**	@brief A function to check if the work for a file is done
*	@param work an fs_work_t pointer, which is the buffer created after fs_read or fs_write
*	@returns a boolean, which represents if the work is completed or not (true if the file work is complete)
* 
*	This function determines if the work from fs_read or fs_write is completed
*/
bool fs_work_is_done(fs_work_t* work);

/**	@brief A function to wait for file work to complete
*	@param work an fs_work_t pointer, which is the buffer created after fs_read or fs_write
*	@returns none
* 
*	Block for the file work to complete. This is how our system
*	can be synchronous as we can block for file work to safely complete.
*/
void fs_work_wait(fs_work_t* work);

/**	@brief A function to check the error code for file work
*	@param work an fs_work_t pointer, which is the buffer created after fs_read or fs_write
*	@returns an integer, which represents the error code for file work
* 
*	Get the error code for the file work.
*	A value of zero generally indicates success.
*	This is used to check that file work was successfully completed
*/
int fs_work_get_result(fs_work_t* work);

/**	@brief A function to get the buffer 
*	@param work an fs_work_t pointer, which is the buffer created after fs_read or fs_write
*	@returns a void*, which contains the buffer of what is written, or what was read
* 
*	Get the buffer associated with the file operation.
*/
void* fs_work_get_buffer(fs_work_t* work);

/**	@brief A function to get the size of the buffer used in file work
*	@param work an fs_work_t pointer, which is the buffer created after fs_read or fs_write
*	@returns a size_t, which represents the size of the buffer used in file work
* 
*	Get the size associated with the file operation
*/
size_t fs_work_get_size(fs_work_t* work);

/**	@brief A function to destroy an fs_work_t object
*	@param work an fs_work_t pointer, which is the buffer created after fs_read or fs_write
*	@returns none
* 
*	Free a file work object, which safely deallocates any allocated memory
*/
void fs_work_destroy(fs_work_t* work);
