/**	@file atomic.h
*	@brief A header file for various atomic operations
* 
*	Atomic operations on 32-bit integers. These functions are useful to make
*	operations thread-safe and prevent the unwanted overriding of variables.
*/
#pragma once


/** @brief A function to atomically increment an integer
*	@param address an integer pointer, which is the address to the integer we are incrementing
*	@returns an integer, which is the old value of the number
* 
*	Increment a number atomically\n
*	Performs the following operation atomically:\n
*	int old_value = *address; (*address)++; return old_value
*/
int atomic_increment(int* address);

/** @brief A function to atomically decrement an integer
*	@param address an integer pointer, which is the address to the integer we are incrementing
*	@returns an integer, which is the old value of the number
* 
*	Decrement a number atomically\n
*	Performs the following operation atomically:\n
*	int old_value = *address; (*address)--; return old_value
*/
int atomic_decrement(int* address);

/** @brief A function to atomically compare integers and assign if equal
*	@param dest an integer pointer, which is the address to the integer we are comparing to compare
*	@param compare an integer, which is the value we compare to *dest
*	@param exchange an integer, which is the value we assign dest if *dest == compare
*	@returns an integer, which is the old value of the number
* 
*	Compare two numbers atomically and assign if equal\n
*	Performs the following operation atomically:\n
*	int old_value = *dest; if (*dest == compare) *dest = exchange; return old_value;
*/
int atomic_compare_and_exchange(int* dest, int compare, int exchange);

/** @brief A function to atomically load an integer and read it
*	@param address an integer pointer, which is the address to the integer we are loading
*	@returns an integer, which is the read parameter
* 
*	Reads an integer from an address\n
*	All writes that occurred before the last atomic_store to this address are flushed\n
*	Paired with atomic_store(int* address, int value), can guarantee ordering and visibility
*/
int atomic_load(int* address);

/** @brief A function to atomically store an integer
*	@param address an integer pointer, which is the address where we want value stored
*	@param value an integer, which is the integer we want stored
*	@returns none
* 
*	Writes an integer\n
*	Paired with an atomic_load(int* address), can guarantee ordering and visibility
*/
void atomic_store(int* address, int value);