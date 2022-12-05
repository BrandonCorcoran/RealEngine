/** @file mutex.h
*	@brief A header file for recursive mutex thread synchronization
* 
*	This set of operations allows for the creation and deletion of mutexes.
*	Locking and unlocking the mutexes are also supported.
*	Mutexes are useful for synchronization and creating thread-safe programs,
*	so that variables and other information are not improperly read or written
*/
#include <stddef.h>

/** @brief A handle to a mutex
* 
*	This will be a type defined by the engine that a Windows mutex will be cast to
*/
typedef struct mutex_t mutex_t;

/**	@brief A function to create a mutex
*	@returns a mutex_t pointer, which is a type defined by our engine, representing the mutex itself.
* 
*	This function creates a mutex, which can be locked or unlocked using the other functions
*	provided by this header.
*/
mutex_t* mutex_create();

/**	@brief A function to destroy a mutex
*	@param mutex a mutex_t pointer, which is a previously created mutex
*	@returns none
* 
*	This function destroys a previously created mutex, calling a Windows-defined function to do so
*/
void mutex_destroy(mutex_t* mutex);

/** @brief A function to lock a mutex
*	@param mutex a mutex_t pointer, which is a previously created mutex
*	@returns none
* 
*	Locks a mutex. May block if another thread unlocks it. Since this is a recursive mutex,
*	if a thread locks a mutex multiple times, it must be unlocked multiple times
*/
void mutex_lock(mutex_t* mutex);

/** @brief A function to unlock a mutex
*	@param mutex a mutex_t pointer, which is a previously created mutex
*	@returns none
* 
*	Unlocks a mutex (allowing it to be locked again, if it has not been locked multiple times)
*/
void mutex_unlock(mutex_t* mutex);
