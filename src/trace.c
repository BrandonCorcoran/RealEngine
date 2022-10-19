#include "trace.h"
#include "timer.h"
#include "fs.h"
#include "queue.h"
#include "atomic.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct trace_event_t		// 28 bytes
{
	char* name;
	int tid;
	UINT64 start_time;
	UINT64 end_time;

} trace_event_t;

typedef struct trace_t				// approx 48 + 8? bytes?
{
	int max_durations_traced;		// passed in
	int num_durations;				// count of durations we have
	bool is_active;					// says when to start capturing data
	heap_t* heap;					// passed in
	char* path;						// file path (its name)	
	fs_t* file;						// file object

	queue_t* queue;					// need some sort of queue
	trace_event_t** all_trace_events;// some sort of array of events? Have their references be in the queue?
} trace_t;


trace_t* trace_create(heap_t* heap, int event_capacity)
{
	// allocate a new trace
	trace_t* trace = heap_alloc(heap, sizeof(trace_t), 8);
	trace->max_durations_traced = event_capacity;
	trace->num_durations = 0;
	trace->is_active = false;
	trace->heap = heap;

	trace->queue = queue_create(trace->heap, trace->max_durations_traced);
	trace->all_trace_events = heap_alloc(trace->heap, trace->max_durations_traced * sizeof(trace_event_t*), 8);

	return trace;
}

void trace_destroy(trace_t* trace)
{
	queue_destroy(trace->queue);
	for (int i = 0; i < trace->num_durations; i++)
	{
		printf("%d\n", sizeof(trace_event_t));
		heap_free(trace->heap, trace->all_trace_events[i]->name);
		heap_free(trace->heap, trace->all_trace_events[i]);
	}
	heap_free(trace->heap, trace->path);
	heap_free(trace->heap, trace->all_trace_events);
	heap_free(trace->heap, trace);
}

void trace_duration_push(trace_t* trace, const char* name)
{
	if (trace->is_active)
	{
		trace_event_t* new_event = heap_alloc(trace->heap, sizeof(trace_event_t), 8);
		
		int name_len = strlen(name);
		new_event->name = heap_alloc(trace->heap, name_len+1, 8);
		strncpy_s(new_event->name, name_len+1, name, name_len);

		new_event->tid = GetCurrentThreadId();
		new_event->start_time = timer_ticks_to_us(timer_get_ticks());
		int temp_index = atomic_increment(&(trace->num_durations));
		trace->all_trace_events[temp_index] = new_event;
		queue_push(trace->queue, trace->all_trace_events[temp_index]);
		
	}
}

void trace_duration_pop(trace_t* trace)
{
	if (trace->is_active)
	{
		trace_event_t* popped_event = queue_pop(trace->queue);
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
	
	trace->is_active = true;
	// set bool to true
}

void trace_capture_stop(trace_t* trace)
{
	char* buf1 = heap_alloc(trace->heap, 10000, 8);
	//for (int i = 0; i < 10000; i++)
	//{
	//	buf1[i] = '\0';
	//}
	char* first_line = "{\n\t\"displayTimeUnit\": \"ns\", \"traceEvents\": [\n\0";
	strncat_s(buf1, 10000, first_line, strlen(first_line));

	for (int i = 0; i < trace->num_durations; i++)
	{
		char temp_buf[1000];	
		int buf1_len = 1000;
		sprintf_s(temp_buf, buf1_len, "\t\t{\"name\":\"%s\",\"ph\":\"B\",\"pid\":0,\"tid\":\"%d\",\"ts\":\"%d\"},\n\0", trace->all_trace_events[i]->name, trace->all_trace_events[i]->tid, trace->all_trace_events[i]->start_time);
		strncat_s(buf1, 10000, temp_buf, strlen(temp_buf));
	}
	for (int i = 0; i < trace->num_durations; i++)
	{
		char temp_buf[1000];
		int buf1_len = 1000;
		if (i + 1 == trace->num_durations)
		{
			sprintf_s(temp_buf, buf1_len, "\t\t{\"name\":\"%s\",\"ph\":\"E\",\"pid\":0,\"tid\":\"%d\",\"ts\":\"%d\"}\n\0", trace->all_trace_events[i]->name, trace->all_trace_events[i]->tid, trace->all_trace_events[i]->end_time);
			strncat_s(buf1, 10000, temp_buf, strlen(temp_buf));
		}
		else
		{
			sprintf_s(temp_buf, buf1_len, "\t\t{\"name\":\"%s\",\"ph\":\"E\",\"pid\":0,\"tid\":\"%d\",\"ts\":\"%d\"},\n\0", trace->all_trace_events[i]->name, trace->all_trace_events[i]->tid, trace->all_trace_events[i]->end_time);
			strncat_s(buf1, 10000, temp_buf, strlen(temp_buf));
		}

	}
	
	char* temp_buf = "\t]\n}\0";
	strncat_s(buf1, 10000, temp_buf, strlen(temp_buf));
	fs_work_t* temp_worker = fs_write(trace->file, trace->path, buf1, strlen(buf1), false);
	// write to file
	fs_work_wait(temp_worker);
	trace->is_active = false;
	// set bool to false
	heap_free(trace->heap, buf1);
}