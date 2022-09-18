#include "heap.h"
#include "debug.h"

#include "tlsf.h"

#include <stddef.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <DbgHelp.h>


typedef struct arena_t {
	pool_t pool;
	struct arena_t* next;

	// additional variables added to an arena to hold the callstack
	void* stack[10];
	IMAGEHLP_SYMBOL64* symbol;
	char* names[10];
} arena_t;

typedef struct heap_t 
{
	tlsf_t tlsf;							// void* that needs memory for its control instructions
	size_t grow_increment;
	arena_t* arena;							// head of linked list
} heap_t;

heap_t* heap_create(size_t grow_increment)
{
	heap_t* heap = VirtualAlloc(NULL, sizeof(heap_t) + tlsf_size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!heap)
	{
		debug_print(
			k_print_error,
			"OUT OF MEMORY!\n"
		);
		return NULL;
	}
	
	heap->grow_increment = grow_increment;	
	heap->tlsf = tlsf_create(heap + 1);	// jump one past heap and then we can find the tlsf_size()
	heap->arena = NULL;

	return heap;
}

// allocated memory and also sets up the arena to store the callstack in the event of a leak
void* heap_alloc(heap_t* heap, size_t size, size_t alignment)
{
	void* address = tlsf_memalign(heap->tlsf, alignment, size);		// << this could fail if tlsf doesn't have enough memory
	if (!address) {
		size_t arena_size = __max(heap->grow_increment, size * 2) + sizeof(arena_t);	// << allocate a little more, since there could be other large requests
		arena_t* arena = VirtualAlloc(NULL, arena_size + tlsf_pool_overhead(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!arena)
		{
			debug_print(
				k_print_error,
				"OUT OF MEMORY!\n"
			);
			return NULL;
		}
		arena->pool = tlsf_add_pool(heap->tlsf, arena + 1, arena_size);

		arena->next = heap->arena;

		for (int i = 0; i < 10; i++) {
			arena->stack[i] = VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			arena->names[i] = VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		}
		
		heap->arena = arena;
		heap->arena->symbol = (IMAGEHLP_SYMBOL64*)VirtualAlloc(NULL, sizeof(IMAGEHLP_SYMBOL64) + 64, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		address = tlsf_memalign(heap->tlsf, alignment, size);		// possible it could still fail
	}

	// Create Stack
	if (address) {
		HANDLE process = GetCurrentProcess();

		SymInitialize(process, NULL, TRUE);
		
		int stack_capacity = 10;
		CaptureStackBackTrace(1, stack_capacity, &heap->arena->stack, NULL);		// will give you an array that is filled with addresses
				
		heap->arena->symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
		heap->arena->symbol->MaxNameLength = 64;

		for (int i = 0; i < 10; i++) {
			if (heap->arena->stack[i] != NULL) {
				SymGetSymFromAddr64(process, heap->arena->stack[i], NULL, heap->arena->symbol);
				strncpy_s(heap->arena->names[i], 64, heap->arena->symbol->Name, 64);
			}
		}
		
		SymCleanup(process);
	}

	return address;
}

void heap_free(heap_t* heap, void* address)
{
	tlsf_free(heap->tlsf, address);
}

static void my_walker(void* ptr, size_t size, int used, void* user)
{
	arena_t* temp_arena = (arena_t*)user;
	if (used) {
		debug_print(
			k_print_error,
			"Memory leak of size %d bytes with callstack:\n",
			(int)size
		);
		for (int i = 0; i < 10; i++) {
			if (temp_arena->names[i] != NULL) {
				printf("[%d] %s\n", i, temp_arena->names[i]);
			}
		}
	}
}

void heap_destroy(heap_t* heap) {	
	arena_t* temp_a = heap->arena;
	while (temp_a) {
		tlsf_walk_pool(temp_a->pool, my_walker, temp_a);
		temp_a = temp_a->next;
	}
	
	tlsf_destroy(heap->tlsf);
	
	arena_t* arena = heap->arena;
	while (arena)
	{
		arena_t* next = arena->next;
		VirtualFree(arena, 0, MEM_RELEASE);
		arena = next;
	}

	VirtualFree(heap, 0, MEM_RELEASE);
}