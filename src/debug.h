/** @file debug.h 
*	@brief A header file for debugging support
* 
*	To implement debugging support, first install the exception handler, using debug_install_exception_handler().
*	Optionally, a mask can be defined to only allow certain prints to 
*	go through. This can be created using debug_set_print_mask(uint32_t mask). 
*	To print messages with this debugging system, debug_print(uint32_t type, _Printf_format_string_ const char* format, ...)
*	should be used. To print a backtrace of the current function callstack, 
*	debug_backtrace(void** stack, int stack_capacity) should be used. The backtrace can also be used
*	to create a tool to find memory leaks.
*/
#pragma once

#include <stdint.h>

/** @brief Flags for printing and creating a mask
* 
*	These flags are used in debug_print(uint32_t type, _Printf_format_string_ const char* format, ...) and debug_set_print_mask(uint32_t mask).
*	They use bitshifted values to allow various types of print messages to 
*	be sent to the console.
*/
typedef enum debug_print_t
{
	k_print_info = 1 << 0,
	k_print_warning = 1 << 1,
	k_print_error = 1 << 2,
} debug_print_t;


/** @brief A function to install the exception handler that will allow for logging messages to the console.
*	@param none
*	@returns none
* 
*	Install unhandled exception handler\n
*	When unhandled exceptions are caught, will log an error and capture a memory dump
*/
void debug_install_exception_handler();

/** @brief A function to set the print mask, which specifies which prints will happen (or be blocked)
*	@param mask a uint32_t, which is a 32-bit integer that uses the above bit-shifting to create a mask
*	@returns none
* 
*	Set mask of which types of prints will actual fire.\n
*	See the debug_print(uint32_t type, _Printf_format_string_ const char* format, ...)\n
*	Additionally, the various enums above can be passed in as parameters and be 'or'ed ( | ) together.
*	This simplifies the interface to not force the user to manually perform the bit-shifting (and instead
*	hiding it behind named constants)
*/
void debug_set_print_mask(uint32_t mask);

/**	@brief A function to print a message to the console
*	@param type a uint32_t, which is a 32-bit integer that specifies which named constant in the enum is used (i.e. info, warning, or error)
*	@param format a _Printf_format_string_ const char*, which is just a formatted string that will be printed
*	@param ... an optional parameter that is dependent on the formatted string
*	@returns none
* 
*	Log a message to the console\n
*	Message may be dropped if type is not in the active mask\n
*	See debug_set_print_mask(uint32_t mask)
*/
void debug_print(uint32_t type, _Printf_format_string_ const char* format, ...);

/** @brief A function that fills a stack with the current function callstack
*	@param stack a pointer to a pointer of void objects (void**), which will be filled by functions on the callstack
*	@param stack_capacity an integer that defines the maximum capacity of the aforementioned stack
*	@returns an integer, which is the number of addresses captured in the stack
* 
*	Capture a list of addresses that make up the current function callstack\n
*	On return, stack contains at most stack_capacity addresses\n
*	This function can be useful in creating a system that detects memory leaks; everytime memory
*	is dynamically allocated, we can capture a backtrace and return backtraces to memory that is 
*	improperly freed.
*/
int debug_backtrace(void** stack, int stack_capacity);