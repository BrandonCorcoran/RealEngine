/**	@file trace.h
*	@brief A header file for creating a Chrome trace for a given thread
* 
*	These set of operations allow for a Chrome trace to be created. Once trace events
*	are done being recorded, they will be written to a file that can then be run in
*	Chrome at Chrome://tracing for easier visualization of where potential 
*	bottlenecks may be in threading.
*/
#pragma once

#include "heap.h"

/** @brief A handle to a heap object
*
*	This will hold all relevant information that is needed for the heap.
*	More about the heap can be found in heap.h. The heap will be used
*	for memory allocation and deallocation.
*/
typedef struct heap_t heap_t;

/**	@brief A handle to a trace object
* 
*	This will hold all relevant information that is needed for the trace system.
*	This will contain information such as the file itself that will be available to 
*	the user.
*/
typedef struct trace_t trace_t;

/**	@brief A function to create a trace object
*	@param heap a heap_t pointer, which will be used to allocate memory for the trace object
*	@param event_capacity an integer, which will be the maximium number of durations that can be traced
*	@returns a pointer to a trace_t object
* 
*	Creates a CPU performance tracing system. This call is necessary to be able to begin
*	tracing durations.
*/
trace_t* trace_create(heap_t* heap, int event_capacity);

/** @brief A function to destroy a trace object
*	@param trace a trace_t pointer, which is the object that stores all information regarding the current trace being created
*	@returns none
*
*	Destroys a CPU performance tracing system. This call is necessary to properly deallocate resources used
*	in the trace object and free any allocated memory.
*/
void trace_destroy(trace_t* trace);

/**	@brief A function to start tracing a duration on the current thread
*	@param trace a trace_t pointer, which is the object that stores all information regarding the current trace being created
*	@param name a const char*, which is the name of this duration.
*	
*	Begin tracing a named duration on the current thread.\n
*	It is okay to nest multiple durations at once\n
*	This function will only have an effect once the start function is
*	called, as the trace must be active and ready to record.
*/
void trace_duration_push(trace_t* trace, const char* name);

/**	@brief A function to stop tracing a duration
*	@param trace a trace_t pointer, which is the object that stores all information regarding the current trace being created
*	@returns none
* 
*	End tracing the currently active duration on the current thread.
*	Since durations can be nested and are stored on a queue, they are removed
*	following FIFO (first in, first out)
*/
void trace_duration_pop(trace_t* trace);

/** @brief A function to start recording trace events
*	@param trace a trace_t pointer, which is the object that stores all information regarding the current trace being created
*	@param path a const char*, which is where the Chrome trace file will be written to.
*	@returns none
* 
*	Start recording trace events. This will allow for durations to be pushed to and popped from.
*	The Chrome trace file created from this is able to be run in Chrome://tracing to visualize
*	how long different threads / processes take.
*/
void trace_capture_start(trace_t* trace, const char* path);

/**	@brief A function to stop recording trace events
*	@param trace a trace_t pointer, which is the object that stores all information regarding the current trace being created
*	@returns none
* 
*	Stop recording trace events. This function writes to the file created in the start function
*/
void trace_capture_stop(trace_t* trace);