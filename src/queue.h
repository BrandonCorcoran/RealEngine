/** @file queue.h
*	@brief A header file for the implementation of a queue
* 
*	This queue implementation is also thread-safe and utilizes FIFO
*	(first-in, first-out) for its elements. In addition to being
*	able to push and pop items, items can also try to be pushed or popped,
*	which is how safety is provided for threads
*/
#pragma once

#include <stdbool.h>

/** @brief A handle to a thread-safe queue
*
*	This will hold all relevant information that is needed for the queue.
*	This includes all the items and head and tail references.
*/
typedef struct queue_t queue_t;

/** @brief A handle to a heap object
*
*	This will hold all relevant information that is needed for the heap.
*	More about the heap can be found in heap.h. The heap will be used
*	for memory allocation and deallocation.
*/
typedef struct heap_t heap_t;

/**	@brief A function to create a queue
*	@param heap a heap_t pointer, which is how memory is managed
*	@param capacity an integer, which defines the allowed capacity of the queue
*	@returns a queue_t pointer, which is how the user can add and remove items from the queue
* 
*	This function creates a queue with the defined capacity.
*	This must be called before attempting to push or pop items.
*/
queue_t* queue_create(heap_t* heap, int capacity);

/**	@brief A function to destroy a queue
*	@param queue a queue_t pointer, which is a previously created queue
*	@returns none
* 
*	Destroy a previously created queue. This needs to be called in order to properly
*	deallocate memory that was allocated to create the queue. Failure to do so will
*	result in memory leaks.
*/
void queue_destroy(queue_t* queue);

/**	@brief A function to add an item to the queue
*	@param queue a queue_t pointer, which is a previously created queue
*	@param item a void pointer, which is what we want to be added to the queue
*	@returns none
* 
*	Push an item onto a queue. If the queue is full, blocks until space is available.
*	Safe for multiple threads to push at the same time.
*/
void queue_push(queue_t* queue, void* item);
 
/**	@brief A function to remove an item from the queue
*	@param queue a queue_t pointer, which is a previously created queue
*	@return a void pointer, which is the item removed from the queue
* 
*	Pop an item off a queue (FIFO order).
*	If the queue is empty, blocks until an item is avaiable.
*	Safe for multiple threads to pop at the same time.
*/
void* queue_pop(queue_t* queue);

/** @brief A function to attempt to push an item to the queue
*	@param queue a queue_t pointer, which is a previously created queue
*	@param item a void pointer, which is what we want to be added to the queue
*	@returns a boolean, which returns true if the item was added; false otherwise
* 
*	Push an item onto a queue if space is available.
*	If the queue is full, returns false.
*	Safe for multiple threads to push at the same time.
*/
bool queue_try_push(queue_t* queue, void* item);

/**	@brief A function to attempt to remove an item from the queue
*	@param queue a queue_t pointer, which is a previously created queue
*	@returns a void pointer, which is the item removed from the queue (if possible; NULL otherwise)
* 
*	Pop an item off a queue (FIFO order).
*	If the queue is empty, returns NULL.
*	Safe for multiple threads to pop at the same time.
*/
void* queue_try_pop(queue_t* queue);