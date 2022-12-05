/** @file timer.h
*	@brief A header file for the support of a high resolution timer
* 
*	These set of functions are used to be able to capture the current time.
*	Additionally, there are functions to support the conversion of these
*	OS ticks to milliseconds or microseconds.
*/
#pragma once

#include <stdint.h>

/**	@brief A function to initialize the timer
*	@returns none
* 
*	Perform one-time initialization of the timer. This can be done in main once.
*	This startup process provides a baseline for where our ticks will be counted from
*/
void timer_startup();

/**	@brief A function to get ticks since startup
*	@returns a uint64_t, which is a 64-bit integer that is the current number of ticks
* 
*	The integer returned is the number of OS-defined ticks that have elapsed since startup.
*	These ticks will be how we determine time, as they will be compared to the startup number
*	of ticks and then converted to milliseconds or microseconds.
*/
uint64_t timer_get_ticks();

/** @brief A function to get the number of ticks per second
*	@returns a uint64_t, which is a 64-bit integer that is the number of ticks per second.
* 
*	Get the OS-defined tick frequency. This function is used inside of startup to give 
*	a conversion factor between ticks and milli-/micro-seconds. It has little use to the 
*	user, as get_ticks or ticks_to_... will provide more relevant timing information.
*/
uint64_t timer_get_ticks_per_second();

/**	@brief A function to convert ticks to microseconds
*	@param t a uint64_t, which is a 64-bit integer that is a number of ticks.
*	@returns a uint64_t, which is a 64-bit integer that is the number of microseconds elapsed
* 
*	Convert a number of OS-defined ticks to microseconds. For the best accuracy, this should be paired
*	with timer_get_ticks(). To do so, pass in timer_get_ticks() as the parameter for this function. This
*	is also useful because it shields the user from having to work with ticks, instead directly converting ticks
*	to microseconds.
*/
uint64_t timer_ticks_to_us(uint64_t t);

/**	@brief A function to convert ticks to milliseconds
*	@param t a uint64_t, which is a 64-bit integer that is a number of ticks.
*	@returns a uint32_t, which is a 32-bit integer that is the number of milliseconds elapsed
*
*	Convert a number of OS-defined ticks to milliseconds. For the best accuracy, this should be paired
*	with timer_get_ticks(). To do so, pass in timer_get_ticks() as the parameter for this function. This
*	is also useful because it shields the user from having to work with ticks, instead directly converting ticks
*	to milliseconds.
*/
uint32_t timer_ticks_to_ms(uint64_t t);