#pragma once

#include <stdlib.h>

//Heap Memory Manager
//
// Main objet, heap_t, represents a dynamic memory heap.
// Once created, memory can be allocated and freed from the heap

// handle to a heap
typedef struct heap_t heap_t;

// Creates a new memory heap
// the grow increment is the default size with chich the heap grows
// should be a multiple of OS page size
heap_t* heap_create(size_t grow_increment);

// allocate memory from a heap
void* heap_alloc(heap_t* heap, size_t size, size_t alignment);

// free memory previously allocated from a heap
void heap_free(heap_t* heap, void* address);

// walker function that uses the user parameter to pass in the arena
// the arena is used to store the callstack
// memory that is still being used is printed as leaked, along with callstack
static void my_walker(void* ptr, size_t size, int used, void* user);

// Destroy a previously created heap
void heap_destroy(heap_t* heap);