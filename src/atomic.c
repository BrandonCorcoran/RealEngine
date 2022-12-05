#include "atomic.h"

/** @brief A built-in Windows definition to import only a portion of built-in Windows functions we will use.
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/** @brief A function to atomically increment an integer
*	@param address an integer pointer
*	@returns an integer
*/
int atomic_increment(int* address)
{
	return InterlockedIncrement(address) - 1;
}

/** @brief A function to atomically decrement an integer
*	@param address an integer pointer
*	@returns an integer
*/
int atomic_decrement(int* address)
{
	return InterlockedDecrement(address) + 1;
}

/** @brief A function to atomically swap two integers
*	@param dest an integer pointer
*	@param compare an integer
*	@param exchange an integer
*	@returns an integer
*/
int atomic_compare_and_exchange(int* dest, int compare, int exchange)
{
	return InterlockedCompareExchange(dest, exchange, compare);
}

/** @brief A function to atomically load an integer
*	@param address an integer pointer
*	@returns an integer
*/
int atomic_load(int* address)
{
	return *(volatile int*)address;
}

/** @brief A function to atomically store an integer
*	@param address an integer pointer
*	@param value an integer
*	@returns none
*/
void atomic_store(int* address, int value)
{
	*(volatile int*)address = value;
}