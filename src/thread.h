/** @file thread.h
*	@brief A header file for multi-threading
* 
*	This set of operations provides thrading support. Threads can be created, destroyed or put to sleep
*/
#pragma once

#include <stddef.h>
#include <stdint.h>

/**	@brief A handle to a thread
* 
*	This will hold all relevant information that will be needed for a thread
*/
typedef struct thread_t thread_t;

/** @brief A function to create a new thread
*	@param function an int (*function)(void*) this is a function that takes a void* as a parameter and what the thread runs
*	@param data a void pointer, which holds any data we would like to be passed along to the thread.
*	@returns a thread_t pointer, which is a pointer to the created thread.
* 
*	On return, the thread begins running the function passed in with the provided data.
*	This allows for multiple functions to be run at once (multi-threading). It is important to use
*	thread-safety measures to ensure that values are properly written / read. Consider seeing atomic.h
*/
thread_t* thread_create(int (*function)(void*), void* data);

/** @brief A function to destroy a thread
*	@param thread a thread_t pointer, which is a reference to a thread object
*	@returns an integer, which is the thread's exit code
* 
*	Waits for a thread to complete and destroys it. This means this function is a blocking call
*	and will only destroy the thread once it finishes running.
*/
int thread_destroy(thread_t* thread);

/** @brief A function to put a thread to sleep
*	@param ms a uint32_t, which is a 32-bit integer representing the desired time to have the thread sleep in milliseconds
*	@returns none
* 
*	Puts the calling thread to sleep for the specified number of milliseconds\n
*	Thread will sleep for *approximately* the specified time.\n
*	This can be used to delay a thread from running.
*/
void thread_sleep(uint32_t ms);