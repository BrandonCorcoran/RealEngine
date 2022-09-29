#include "atomic.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int atomic_increment(int* address) {
	return InterlockedIncrement(address);
}

int atomic_decrement(int* address) {
	return InterlockedDecrement(address);
}

int atomic_compare_and_exchange(int* dest, int compare, int exchange) {
	return InterlockedCompareExchange(dest, exchange, compare);
}

int atomic_load(int* address)
{
	return *(volatile int*)address;
}

void atomic_store(int* address, int value)
{
	*(volatile int*)address = value;
}