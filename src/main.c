// c:> RealEngine.exe foo bar

#include "heap.h"
#include "debug.h"
#include "wm.h"
#include "thread.h"
#include "atomic.h"
#include "mutex.h"

#include <stdio.h>
#include <excpt.h>

#include <windows.h>

static void homework1_test();

// NOTES ON MUTEX / THREADS / ATOMIC - 1

int main(int argc, const char* argv[])
{
	debug_install_exception_handler();
	homework1_test();

	// NOTES ON DEREFERENCING NULL, TRY / EXCEPT, HEAP - 2

	debug_set_print_mask(k_print_warning | k_print_error);

	heap_t* heap = heap_create(2 * 1024 * 1024);
	wm_window_t* window = wm_create(heap);

	// NOTES ON THREADS / MUTEX - 3

	// THIS IS THE MAIN LOOP!
	while (!wm_pump(window))
	{
		int x, y;
		uint32_t mask = wm_get_mouse_mask(window);
		wm_get_mouse_move(window, &x, &y);
		debug_print(
			k_print_info,
			"MOUSE mask = %x move=%dx%x\n",
			mask,
			x, y);
	}

	wm_destroy(window);
	heap_destroy(heap);

	return 0;
}


static void* homework1_allocate_1(heap_t* heap)
{
	return heap_alloc(heap, 16 * 1024, 8);
}

static void* homework1_allocate_2(heap_t* heap)
{
	return heap_alloc(heap, 256, 8);
}

static void* homework1_allocate_3(heap_t* heap)
{
	return heap_alloc(heap, 32 * 1024, 8);
}

static void homework1_test()
{
	heap_t* heap = heap_create(4096);
	void* block1 = homework1_allocate_1(heap);
	/*leaked*/ homework1_allocate_2(heap);
	/*leaked*/ homework1_allocate_3(heap);

	heap_free(heap, block1);
	heap_destroy(heap);
}


// These are my personal notes from class that I have moved to the bottom of the file.
// I feel that since they are out of the way this should not be a problem in terms of the coding standard
// but please let me know if that is not the case.

// NOTES ON MUTEX / THREADS / ATOMIC - 1
//typedef struct thread_data_t
//{
//	int* counter;
//	mutex_t* mutex;
//} thread_data_t;
//
//static int thread_function(void* data)
//{
//	int* counter = data;
//	thread_data_t* thread_data = data;
//	for (int i = 0; i < 100000; i++) 
//	{
//		// ATOMIC EXAMPLE
//		//atomic_increment(counter);			// makes this an atomic operation
//
//		//volatile int value = *counter;													// volatile keyword - need to read from counter to use its value; if we just used *counter
//		//																				// read counter three times in the args, so it could change if we do not hold its value initially
//		//																				// compiler doesn't know about / care about threads, so it can see int value = *counter and replace instances of value
//		//																				// with *counter; we must convince compiler it must read when we say it has to (this is what volatile keyword means)
//		//while (atomic_compare_and_exchange(counter, value, value + 1) != value);		// eventually becomes slow (not optimal way of doing)
//		//																				// tries again and again and again
//
//		// MUTEX EXAMPLE
//		mutex_lock(thread_data->mutex);
//		(*thread_data->counter) = *thread_data->counter + 1;
//		mutex_unlock(thread_data->mutex);
//
//		//(*counter)++;
//	}
//	return 0;
//}


// NOTES ON DEREFERENCING NULL, TRY / EXCEPT, HEAP - 2
	// Example of dereferencing NULL
	/*int* foo = NULL;
	*foo = 42;*/

	//__try
	//{
	//	int* foo = NULL;
	//	*foo = 42;							// debugger will catch, not us; can disable in Exception Settings Window; Access Violation switch off for us to manually handle
	//}
	//__except (EXCEPTION_EXECUTE_HANDLER)
	//{
	//	debug_print_line(k_print_error, "AA!");
	//}


	//heap_t* heap = heap_create(2 * 1024 * 1024);			// at least 2 MB
	//char* ptr = heap_alloc(heap, 4353, 32);
	//
	//for (int i = 0; i < 4353; ++i) {
	//	ptr[i] = i;
	//}
	//
	//heap_free(heap, ptr);
	//heap_destroy(heap);


// NOTES ON THREADS / MUTEX - 3

#if 0
int counter = 0;
int counters[8] = { 0 };
thread_t* threads[8];
for (int i = 0; i < _countof(threads); i++) {
	threads[i] = thread_create(thread_function, &counters[i]);

}

//thread_t* thread = thread_create(thread_function, heap);		// this can run at same time as what happens below
for (int i = 0; i < _countof(threads); i++) {
	// may have 8 threads running at once;
	thread_destroy(threads[i]);									// allow for concurrency
	counter += counters[i];										// sidesteps problem by not having shared memory; then at join point, we combine work from across all threads
																// fastest way by far
}

// value should be 800,000; it is definitely not printing 800,000
#endif

	/*thread_data_t thread_data =
	{
		.counter = &counter,
		.mutex = mutex_create(),
	};*/

	/*int counter = 0;
	thread_t* threads[8];
	for (int i = 0; i < _countof(threads); i++) {
		threads[i] = thread_create(thread_function, &counter);

	}*/

	////thread_t* thread = thread_create(thread_function, heap);		// this can run at same time as what happens below
	//for (int i = 0; i < _countof(threads); i++) {
	//	// may have 8 threads running at once;
	//	thread_destroy(threads[i]);									// allow for concurrency
	//}

	//mutex_destroy(thread_data.mutex);
	//// value should be 800,000; it is definitely not printing 800,000


	//debug_print_line(0, "foo=%d", "foo");