/**	@file semaphore.h
*	@brief A header file for the implementation of counting semaphore thread synchronization
* 
*	These set of operations allow for the creation and deletion of a counting semaphore.
*	Once created, the semaphore can be acquired and released. Similar to a mutex, a semaphore
*	allows for an asset to be acquired by n "acquirers", where n is the max_count of the semaphore.
*/
#pragma once

#include <stdbool.h>

/** @brief A handle to a semaphore
* 
*	This will hold all relevant information that is needed for the semaphore, including its current and max count.
*/
typedef struct semaphore_t semaphore_t;

/** @brief A function that creates a new semaphore
*	@param initial_count an integer, which represents the starting count of the semaphore (generally should start as max_count, so it can be acquired)
*	@param max_count an integer, which represents the maximum that the semaphore can be released up to
*	@returns a semaphore_t pointer, which is what will be passed into the acquire and release functions
* 
*	Creates a new semaphore. This uses a Windows-defined function to create a semaphore, which is then returned
*	as the semaphore_t type
*/
semaphore_t* semaphore_create(int initial_count, int max_count);

/**	@brief A function that destroys a semaphore
*	@param semaphore a semaphore_t pointer, which is the previously-created semaphore (from create)
*	@returns none
* 
*	Destroys a previously created semaphore. This is necessary to internally call the proper function to clean 
*	up the semaphore
*/
void semaphore_destroy(semaphore_t* semaphore);

/** @brief A function that acquires the semaphore
*	@param semaphore a semaphore_t pointer, which is the previously-created semaphore (from create)
*	@returns none
* 
*	Lowers the semaphore count by one.
*	If the semaphore count is zero, blocks until another thread releases.
*/
void semaphore_acquire(semaphore_t* semaphore);

/** @brief A function that attempts to acquire the semaphore
*	@param semaphore a semaphore_t pointer, which is the previously-created semaphore (from create)
*	@returns a boolean that represents whether or not the semaphore can be acquired (true means the semaphore can be acquired)
* 
*	Attempts to lower the semaphore count by one. If the semaphore count is zero, returns false. Otherwise true.
*	This function can be useful instead of calling semaphore_acquire(semaphore_t* semaphore), which will block 
*	if the count is zero. If the user does not need to block, then just trying to acquire the semaphore can prevent that
*	block from happening.
*/
bool semaphore_try_acquire(semaphore_t* semaphore);

/** @brief A function that releases the semaphore
*	@param semaphore a semaphore_t pointer, which is the previously-created semaphore (from create)
*	@returns none
* 
*	Raises the semaphore count by one, which makes it available to be used elsewhere.
*	Any thread waiting for this resource that is currently blocking will have "first priority"
*	to acquire the semaphore.
*/
void semaphore_release(semaphore_t* semaphore);