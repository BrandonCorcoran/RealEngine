/** @file event.h
*	@brief A header file for event-based thread synchronization
* 
*	Events can be created and destroyed.
*	Threads can wait on an event, which can be signaled
*	and results in any waiting threads to resume
*/
#pragma once

#include <stdbool.h>

/**	@brief A handle to an event
*/
typedef struct event_t event_t;

/**	@brief A function to create a new event
*	@returns an event_t pointer, which is the event
*	that will be able to be signaled or waited on
* 
*	Creates a new event.
*	This must be destroyed before program termination and is created
*	using built-in Windows functions.
*/
event_t* event_create();

/**	@brief A function to destroy an event
*	@param event an event_t pointer, which is a previously created event
*	@returns none
* 
*	This function destroys a previously created event.
*	It uses a built-in Windows function to do so.
*/
void event_destroy(event_t* event);

/**	@brief A function to signal an event
*	@param event an event_t pointer, which is a previously created event
*	@returns none
*	
*	This function signals an event, so that all threads waiting on this event
*	will now resume. In tandem with event_wait(event_t* event), this
*	allows for a way of blocking a thread until the 
*	event is signaled.
*/
void event_signal(event_t* event);

/**	@brief A function to wait for an event to be signaled
*	@param event an event_t pointer, which is a previously created event
*	@returns none
*	
*	This function waits for an event to be signaled. This acts as a block
*	for until the event is signaled using event_signal(event_t* event)
*/
void event_wait(event_t* event);

/**	@brief A function to check if an event is signaled
*	@param event an event_t pointer, which is a previously created event
*	@returns a boolean, which represents if the event has been signaled (true, otherwise false)
* 
*	Determine if an event is signaled. This can be used to determine if a thread is 
*	fully completed (i.e. if the event has been signaled then it is no longer
*	blocking because of the event)
*/
bool event_is_raised(event_t* event);