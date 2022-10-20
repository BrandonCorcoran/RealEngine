#include "trace.h"
#include "timer.h"
#include "fs.h"
#include "queue.h"
#include "atomic.h"
#include "semaphore.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// a struct holding each event, its name, start/end times, and thread id
typedef struct trace_event_t		// 28 bytes
{
	UINT64 start_time;
	UINT64 end_time;
	char* name;
	int tid;
} trace_event_t;

// a struct holding the relevant variables needed for a trace
typedef struct trace_t
{
	int max_durations_traced;		// passed in
	int num_durations;				// count of durations we have
	bool is_active;					// says when to start capturing data
	heap_t* heap;					// passed in
	char* path;						// file path (its name)	
	fs_t* file;						// file object

	semaphore_t* semaphore;			// semaphore to ensure thread safety

	queue_t* queue;					// need some sort of queue
	trace_event_t** all_trace_events;// some sort of array of events? Have their references be in the queue?
} trace_t;


trace_t* trace_create(heap_t* heap, int event_capacity)
{
	// allocate a new trace and initialize variables
	trace_t* trace = heap_alloc(heap, sizeof(trace_t), 8);
	trace->max_durations_traced = event_capacity;
	trace->num_durations = 0;
	trace->is_active = false;
	trace->heap = heap;

	// create queue that will hold a combined total of event_capacity pushes and pops
	trace->queue = queue_create(trace->heap, trace->max_durations_traced);

	// create the first layer of a 2D structure that will hold all trace_events
	trace->all_trace_events = heap_alloc(trace->heap, trace->max_durations_traced * sizeof(trace_event_t*), 8);

	trace->semaphore = semaphore_create(1, 1);

	return trace;
}

void trace_destroy(trace_t* trace)
{
	queue_destroy(trace->queue);
	fs_destroy(trace->file);
	for (int i = 0; i < trace->num_durations; i++)
	{
		heap_free(trace->heap, trace->all_trace_events[i]->name);
		heap_free(trace->heap, trace->all_trace_events[i]);
	}
	heap_free(trace->heap, trace->path);
	heap_free(trace->heap, trace->all_trace_events);
	semaphore_destroy(trace->semaphore);
	heap_free(trace->heap, trace);
}

void trace_duration_push(trace_t* trace, const char* name)
{
	if (trace->is_active)
	{
		// for each push, we create a new event and initialize its variables
		trace_event_t* new_event = heap_alloc(trace->heap, sizeof(trace_event_t), 8);
		
		int name_len = strlen(name);
		new_event->name = heap_alloc(trace->heap, name_len+1, 8);
		strncpy_s(new_event->name, name_len+1, name, name_len);

		new_event->tid = GetCurrentThreadId();
		new_event->start_time = timer_ticks_to_us(timer_get_ticks());

		// to prevent the index here from being improperly incremented with multiple threads running, we use atomic_increment()
		int temp_index = atomic_increment(&(trace->num_durations));
		// add the event to the 2D array of all lists
		trace->all_trace_events[temp_index] = new_event;
		// push the event on the queue
		queue_push(trace->queue, trace->all_trace_events[temp_index]);
		
	}
}

void trace_duration_pop(trace_t* trace)
{
	if (trace->is_active)
	{
		// for thread safety, we implement a semaphore, so only one event is popped at a time.
		semaphore_acquire(trace->semaphore);
		trace_event_t* popped_event = queue_pop(trace->queue);
		semaphore_release(trace->semaphore);
		popped_event->end_time = timer_ticks_to_us(timer_get_ticks());
	}
}

void trace_capture_start(trace_t* trace, const char* path)
{
	int path_len = strlen(path);
	trace->path = heap_alloc(trace->heap, path_len+1, 8);
	strncpy_s(trace->path, path_len+1, path, path_len);

	fs_t* fs = fs_create(trace->heap, 16);
	
	// Create file
	trace->file = fs;
	
	// set bool to true
	trace->is_active = true;
}

void trace_capture_stop(trace_t* trace)
{
	// allocate a buffer that will hold all of the output for the json file and write to the file at the end.
	char* buf1 = heap_alloc(trace->heap, 10000, 8);
	char* first_line = "{\n\t\"displayTimeUnit\": \"ns\", \"traceEvents\": [\n\0";
	strncat_s(buf1, 10000, first_line, strlen(first_line));

	for (int i = 0; i < trace->num_durations; i++)
	{
		char temp_buf[1000];	
		int buf1_len = 1000;
		sprintf_s(temp_buf, buf1_len, "\t\t{\"name\":\"%s\",\"ph\":\"B\",\"pid\":0,\"tid\":\"%d\",\"ts\":\"%I64u\"},\n\0", trace->all_trace_events[i]->name, trace->all_trace_events[i]->tid, trace->all_trace_events[i]->start_time);
		strncat_s(buf1, 10000, temp_buf, strlen(temp_buf));
	}
	for (int i = 0; i < trace->num_durations; i++)
	{
		char temp_buf[1000];
		int buf1_len = 1000;
		if (i + 1 == trace->num_durations)
		{
			sprintf_s(temp_buf, buf1_len, "\t\t{\"name\":\"%s\",\"ph\":\"E\",\"pid\":0,\"tid\":\"%d\",\"ts\":\"%I64u\"}\n\0", trace->all_trace_events[i]->name, trace->all_trace_events[i]->tid, trace->all_trace_events[i]->end_time);
			strncat_s(buf1, 10000, temp_buf, strlen(temp_buf));
		}
		else
		{
			sprintf_s(temp_buf, buf1_len, "\t\t{\"name\":\"%s\",\"ph\":\"E\",\"pid\":0,\"tid\":\"%d\",\"ts\":\"%I64u\"},\n\0", trace->all_trace_events[i]->name, trace->all_trace_events[i]->tid, trace->all_trace_events[i]->end_time);
			strncat_s(buf1, 10000, temp_buf, strlen(temp_buf));
		}

	}
	
	char* temp_buf = "\t]\n}\0";
	strncat_s(buf1, 10000, temp_buf, strlen(temp_buf));

	// write to file and wait for this write to complete before destroying the buffer 
	fs_work_t* temp_worker = fs_write(trace->file, trace->path, buf1, strlen(buf1), false);
	fs_work_wait(temp_worker);
	// set bool to false
	trace->is_active = false;
	heap_free(trace->heap, buf1);
	fs_work_destroy(temp_worker);
}