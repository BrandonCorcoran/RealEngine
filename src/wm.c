#include "wm.h"

#include "heap.h"
#include "debug.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

// PADDING EXAMPLE
// ---------------
// enum color_t {
//	k_red,
//	k_blue,
// };

// typedef struct table_t
// {
//	bool is_flat;				1 byte
//								3 bytes of padding?
//	float leg_length_meters;	4 bytes
//	color_t color;				4 bytes						<< ENUMs are usually 4 bytes (can control, but generally this)
// } table_t;

//															Order matters in padding; if the bool was at the end, we would not need padding
// sizeof(table_t) --> 12 bytes

typedef struct wm_window_t {
	HWND hwnd;
	heap_t* heap;
	bool quit;
	bool has_focus;
	// 2 bytes of char padding here, so size goes up to 28 bytes
	uint32_t mouse_mask;	// 32 bits; assign special meaning to each of the bits (i.e. 0 bit is left mouse, 1 bit is right mouse, 2 bit is middle mouse)
							// ex. middle mouse down could be 100; 101 would be middle and left mouse button down; releasing left mouse button would be 100
	uint32_t key_mask;
	int mouse_x;
	int mouse_y;
} wm_window_t;


wm_window_t A_WINDOW;


const struct {						// want the constant to be an array
	int virtual_key;
	int real_engine_key;
}
k_key_map[] =						// just assigning these variables to k_key_map[] in one easy move
{
	{.virtual_key = VK_LEFT, .real_engine_key = k_key_left, },
	{.virtual_key = VK_RIGHT, .real_engine_key = k_key_right,  },
	{.virtual_key = VK_UP, .real_engine_key = k_key_up,  },
	{.virtual_key = VK_DOWN, .real_engine_key = k_key_down,  },
};


static LRESULT CALLBACK _window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wm_window_t* win = (wm_window_t*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (win) {
		switch (uMsg)
		{
		case WM_KEYDOWN:
			for (int i = 0; i < _countof(k_key_map); ++i) {				// countof will give us the size of this (otherwise we have to get legnth)
																		// otherwise we have to divide by each element size
				if (k_key_map[i].virtual_key == wParam) {
					win->key_mask |= k_key_map[i].real_engine_key;
					break;
				}
			}
			break;
		case WM_KEYUP:
			for (int i = 0; i < _countof(k_key_map); ++i) {				// countof will give us the size of this (otherwise we have to get legnth)
																		// otherwise we have to divide by each element size
				if (k_key_map[i].virtual_key == wParam) {
					win->key_mask &= ~k_key_map[i].real_engine_key;
					break;
				}
			}
			break;

		case WM_LBUTTONDOWN:		// left mouse button down
			win->mouse_mask |= k_mouse_button_left;			// bitwise OR operation; similar to union operation
															// can also write as s_mouse_mask = s_mouse_mask | k_mouse_button_left
			break;
		case WM_LBUTTONUP:			// left mouse button up
			win->mouse_mask &= ~k_mouse_button_left;			// probably true that 0 bit is high (a 1), and we just want to clear this 0 bit
															// we want everything but the 0th bit; keep everything that was high, high 
															// bitwise XOR --> ^
															// ex. 001 ^ 001 = 000				<<< slight issue is we don't want to trust windows we were given a 1 (don't want to toggle)
			break;
		case WM_RBUTTONDOWN:		// right mouse button down
			win->mouse_mask |= k_mouse_button_right;
			break;
		case WM_RBUTTONUP:			// right mouse button up
			win->mouse_mask &= ~k_mouse_button_right;
			break;
		case WM_MBUTTONDOWN:		// middle mouse button down
			win->mouse_mask |= k_mouse_button_middle;
			break;
		case WM_MBUTTONUP:			// middle mouse button up
			win->mouse_mask &= ~k_mouse_button_middle;
			break;

		case WM_MOUSEMOVE:
			//printf("%dx%d\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			if (win->has_focus) {
				// Relative mouse movement in four steps:
				// 1. Get current mouse position (old_cursor)
				// 2. Move mouse back to center of window.
				// 3. Get current mouse position (new_cursor)
				// 4. Compute relative movement (old_cursor - new_cursor)

				POINT old_cursor;
				GetCursorPos(&old_cursor);				// window-space

				RECT window_rect;						// returns position of window on the screen CurserSae
				GetWindowRect(hwnd, &window_rect);
				SetCursorPos(
					(window_rect.left, +window_rect.right) / 2, 
					(window_rect.top, +window_rect.bottom) / 2);

				POINT new_cursor;
				GetCursorPos(&new_cursor);

				win->mouse_x = old_cursor.x - new_cursor.x;
				win->mouse_y = old_cursor.y - new_cursor.y;
			}
			break;

		case WM_ACTIVATEAPP:			// True - focus; False - lose focus
			ShowCursor(!wParam);		// hides cursor
			win->has_focus = wParam;		// only does locally
			break;

		case WM_CLOSE:
			win->quit = true;
			break;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

wm_window_t* wm_create(heap_t* heap)
{
	WNDCLASS wc =
	{
		.lpfnWndProc = _window_proc,
		.hInstance = GetModuleHandle(NULL),
		.lpszClassName = L"Real Engine window class",
	};
	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		L"Real Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		wc.hInstance,
		NULL);

	if (!hwnd) {
		debug_print(
			k_print_warning,
			"no window\n"
		);
		return NULL;
	}

	wm_window_t* win = heap_alloc(heap, sizeof(wm_window_t), 8);		// could use calloc
	win->has_focus = false;
	win->hwnd = hwnd;
	win->key_mask = 0;
	win->mouse_mask = 0;
	win->quit = false;
	win->heap = heap;

	// ADDED AS TEMP
	void* stack[10];
	debug_backtrace(stack, _countof(stack));


	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)win);

	// Windows are created hidden by default, so we need to show it here
	ShowWindow(hwnd, TRUE);

	return win;
}

bool wm_pump(wm_window_t* window)
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return window->quit;
}

uint32_t wm_get_mouse_mask(wm_window_t* window) {
	return window->mouse_mask;
}

uint32_t wm_get_key_mask(wm_window_t* window) {
	return window->key_mask;
}

void wm_get_mouse_move(wm_window_t* window, int* x, int* y) {
	*x = window->mouse_x;
	*y = window->mouse_y;
}

void wm_destroy(wm_window_t* window)
{
	DestroyWindow(window->hwnd);
	heap_free(window->heap, window);
}
