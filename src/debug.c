/** \file */
#include "debug.h"

#include <stdarg.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <DbgHelp.h>

static uint32_t s_mask = 0xffffffff;						// all the bits are set to 1

static LONG debug_exception_handler(LPEXCEPTION_POINTERS ExceptionInfo)				// MORE PARAMS
{
	debug_print(k_print_error, "Caught exception!\n");

	HANDLE file = CreateFile(L"real_engine-crash.dmp", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mini_exception = { 0 };
		mini_exception.ThreadId = GetCurrentThreadId();
		mini_exception.ExceptionPointers = ExceptionInfo;
		mini_exception.ClientPointers = FALSE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithThreadInfo, &mini_exception, NULL, NULL);
		CloseHandle(file);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void debug_install_exception_handler()
{
	//SetUnhandledExceptionFilter(debug_exception_handler);
	AddVectoredExceptionHandler(TRUE, debug_exception_handler);
}


void debug_set_print_mask(uint32_t mask)
{
	s_mask = mask;
}


void debug_print(uint32_t type, _Printf_format_string_ const char* format, ...)
{
	//debug_print_line(0, "foo=%s", 42);					// problem because we are passing a number and will be interpreted as a string (will crash)

	if ((s_mask & type) == 0)								// bitwise and; is the first bit in the mask?
	{
		return;
	}

	va_list args;
	va_start(args, format);									// take all variable arguments and stick them into args
	char buffer[256];										// C has unsafe functions in standard library
	vsnprintf(buffer, sizeof(buffer), format, args);		// pass to printf function that will convert them to a string; this was added as "safe" function to prevent overflow
	va_end(args);

	OutputDebugStringA(buffer);

	DWORD bytes = (DWORD)strlen(buffer);
	DWORD written = 0;
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(out, buffer, bytes, &written, NULL);

}

int debug_backtrace(void** stack, int stack_capacity)
{
	return CaptureStackBackTrace(1, stack_capacity, &stack, NULL);		// will give you an array that is filled with addresses
}

// each line below is a diff frame
// main ->
// foo ->
// bar ->
// baz -> CaptureStackBackTrace()		<< this will give baz, bar, foo, main

// can say how many frames to capture / to skip some
// also need to give it some memory