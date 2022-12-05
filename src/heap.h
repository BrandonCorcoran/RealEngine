/** @file heap.h
*	@brief A header file for creating a heap memory manager
* 
*	Main object, heap_t, represents a dynamic memory heap. Once
*	created, memory can be allocated and freed from the heap.
*/
#pragma once

#include <stdlib.h>

/** @brief A handle to a heap object
*
*	This will hold all relevant information that is needed for the heap.
*	The heap will be used for memory allocation and deallocation.
*/
typedef struct heap_t heap_t;

/** @brief A function to create the memory heap
*	@param grow_increment a size_t, which defines the default size with which the heap grows
*	@returns a heap_t pointer, which is the heap that will be used to allocate and deallocate memory
* 
*	Creates a new memory heap. This is required to be created in order to allocate / deallocate memory\n
*	Note that the grow_increment should be a multiple of the OS page size.
*/
heap_t* heap_create(size_t grow_increment);

/** @brief A function to allocate memory from the heap
*	@param heap a heap_t pointer, which is the already allocated heap
*	@param size a size_t, which is the size of memory desired to be allocated
*	@param alignment a size_t, which is the byte alignment. Generally, this should be left as 8
*	@returns a void pointer, which is the address of the allocated chunk of memory
* 
*	This function allocates a user-definied size of memory. It must be freed before the program terminates
*	or a memory leak will result. Allocating memory is achieved through built-in Windows functions for 
*	memory allocation
*/
void* heap_alloc(heap_t* heap, size_t size, size_t alignment);

/**	@brief A function to free previously allocated memory on the heap
*	@param heap a heap_t pointer, which is the already allocated heap
*	@param address a void pointer, which is the address of the allocated memory that we want to free
*	@returns none
*	
*	This function frees memory prevoiusly allocated from a heap. This is done through built-in Windows functions 
*	for memory deallocation. Allocated memory must be deallocated to prevent memory leaks
*/
void heap_free(heap_t* heap, void* address);

// SINCE THIS FUNCTION IS ONLY USED WITHIN heap.c AND IS STATIC, IT DOES NOT NEED TO BE PUBLIC FACING
// walker function that uses the user parameter to pass in the arena
// the arena is used to store the callstack
// memory that is still being used is printed as leaked, along with callstack
//static void my_walker(void* ptr, size_t size, int used, void* user);

/** @brief A function to destroy a previously created heap
*	@param heap a heap_t pointer, which is an already allocated heap
*	@returns none
* 
*	This function destroys a previously created heap. As the heap itself is virtually allocated,
*	it too must be properly deallocated. This is also where memory leaks can be detected, using TLSF and
*	the my_walker(void* ptr, size_t size, int used, void* user) function. If a stack backtrace was captured
*	it can also be printed here to show where memory leaks remain.
*/
void heap_destroy(heap_t* heap);
