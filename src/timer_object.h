/**	@file timer_object.h
*	@brief A header file for a timer object
* 
*	This set of operations supports pausing/resuming time,
*	scaling time (slowing, speeding up), and a parent-child
*	relationship of time where the child inherits the parent's
*	base time.
*/
#pragma once

#include "heap.h"

#include <stdint.h>

/**	@brief A handle to a time object
*	
*	This holds all relevant information needed for a timer,
*	including the current ticks, the change in ticks, a 
*	parent, if applicable, and if the timer is paused or not.
*/
typedef struct timer_object_t timer_object_t;

/**	@brief A function to create a time object
*	@param heap a pointer to a heap_t object, which is how memory is managed
*	@param parent a pointer to a timer_object_t object, which is optional for the created time object
*	to have a parent
*	@returns a timer_object_t pointer, which is the child of the optional parent and a new time object
* 
*	This function creates a new time object with the defined parent.
*	If the parent is NULL, use the system timer as base time.
*	This function call is necessary to be able to use the following functions
*/
timer_object_t* timer_object_create(heap_t* heap, timer_object_t* parent);

/**	@brief A function to destroy a time object
*	@param t a pointer to a timer_object_t, which is a previously created time object
*	@returns none
*	
*	This function destroys a previously created time object.
*	This is necessary to avoid any memory leaks from previously allocated memory
*/
void timer_object_destroy(timer_object_t* t);

/**	@brief A function to update a time object
*	@param t a pointer to a timer_object_t, which is a previously created time object
*	@returns none
* 
*	This function provides support for a per-frame update
*	for a time object. The current time and delta
*	time are both updated. THis is accomplished by looking
*	at the timer's parent (if it has one) and calculating the
*	ticks
*/
void timer_object_update(timer_object_t* t);

/**	@brief A function to get the current time in microseconds
*	@param t a pointer to a timer_object_t, which is a previously created time object
*	@returns a uint64_t, which is the converted ticks in microseconds
* 
*	This function gets the current time in microseconds,
*	using timer.h functions to convert the current ticks
*	to microseconds
*/
uint64_t timer_object_get_us(timer_object_t* t);
	
/**	@brief A function to get the current time in milliseconds
*	@param t a pointer to a timer_object_t, which is a previously created time object
*	@returns a uint32_t, which is the converted ticks in milliseconds
*
*	This function gets the current time in milliseconds,
*	using timer.h functions to convert the current ticks
*	to milliseconds
*/
uint32_t timer_object_get_ms(timer_object_t* t);

/**	@brief A function to get the frame time in microseconds
*	@param t a pointer to a timer_object_t, which is a previously created time object
*	@returns a uint64_t, which is the converted ticks in microseconds
*
*	This function gets the frame time in microseconds,
*	using timer.h functions to convert the delta ticks
*	to microseconds
*/
uint64_t timer_object_get_delta_us(timer_object_t* t);

/**	@brief A function to get the frame time in milliseconds
*	@param t a pointer to a timer_object_t, which is a previously created time object
*	@returns a uint32_t, which is the converted ticks in milliseconds
*
*	This function gets the frame time in milliseconds,
*	using timer.h functions to convert the delta ticks
*	to milliseconds
*/
uint32_t timer_object_get_delta_ms(timer_object_t* t);

/**	@brief A function to set the time scale
*	@param t a pointer to a timer_object_t, which is a previously created time object
*	@param s a float, which is the desired time scale to be used
*	@returns none
* 
*	This function sets the time scale value. A smaller value
*	slows down time, while a larger value speeds it up. A value
*	of 1.0f is normal speed.
*/
void timer_object_set_scale(timer_object_t* t, float s);

/**	@brief A function to pause time
*	@param t a pointer to a timer_object_t, which is a previously created time object
*	@returns none
* 
*	This function sets the boolean in the timer_object_t
*	to be true, which pauses time.
*/
void timer_object_pause(timer_object_t* t);

/**	@brief A function to resume time
*	@param t a pointer to a timer_object_t, which is a previously created time object
*	@returns none
* 
*	This function resumes previously paused time by setting
*	the boolean in the timer_object_t to false and
*	setting the bias ticks to the parent's current ticks
*	(assuming there is a parent)
*/
void timer_object_resume(timer_object_t* t);