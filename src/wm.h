/**	@file wm.h
*	@brief A header file for a window manager
* 
*	Main object is wm_window_t to represent a single OS-level window.
*	Window should be pumped every frame.
*	After pumping, a window can be queried for user input.
*	Generally, this is used in tandem with GPU-related / rendering functions
*	(in simple_game or frogger_game) to display graphics in the window
*/
#include <stdbool.h>
#include <stdint.h>

/**	@brief A handle to a window manager window object
*
*	This will hold all relevant information that is needed for the window
*	that objects will be rendered in. This window is where the game will be
*	displayed
*/
typedef struct wm_window_t wm_window_t;

/** @brief A handle to a heap object
*
*	This will hold all relevant information that is needed for the heap.
*	More about the heap can be found in heap.h. The heap will be used
*	for memory allocation and deallocation.
*/
typedef struct heap_t heap_t;

/**	@brief This enum is for mouse buttons
* 
*	For more info look to wm_get_mouse_mask(wm_window_t* window)
*	3 bits are used to represent each the left, right, and 
*	middle mouse button
*/
enum {
	k_mouse_button_left = 1 << 0,		// first bit
	k_mouse_button_right = 1 << 1,		// second bit
	k_mouse_button_middle = 1 << 2,		// third bit
};

/**	@brief This enum is for keyboard keys
*
*	For more info look to wm_get_key_mask(wm_window_t* window)
*	4 bits are used to represent each the left, right, up, and
*	down keys (used for movement)
*/
enum {
	k_key_up = 1 << 0,			// first bit
	k_key_down = 1 << 1,		// second bit
	k_key_left = 1 << 2,		// third bit
	k_key_right = 1 << 3,		// fourth bit
};

/**	@brief A function to create a new window
*	@param heap a pointer to a heap_t object, which is how memory is managed
*	@returns a wm_window_t pointer, which is how we will be able to pump
*	it and query it
* 
*	Creates a new window. Must be destroyed with wm_destroy(wm_window_t* window)
*	Returns NULL on failure, otherwise a new window
*/
wm_window_t* wm_create(heap_t* heap);

/**	@brief A function to destroy a window
*	@param window a pointer to a wm_window_t, which is a previously created window
*	@returns none
* 
*	The window must be destroyed before program termination
*/
void wm_destroy(wm_window_t* window);

/**	@brief A function to pump the window
*	@param window a pointer to a wm_window_t, which is a previously created window
*	@returns a boolean that represents whether the window is open or was quit out of (true if quit; false if
*	still using)
* 
*	Messages (and rendering) for the window are pumped.
*	This will refresh the mouse and key state on the window
*/
bool wm_pump(wm_window_t* window);

/**	@brief A function to get the mask for the mouse
*	@param window a pointer to a wm_window_t, which is a previously created window
*	@returns a uint32_t, which represents the 3 bits in the enum in this file
* 
*	Get a mask of all mouse buttons currently held.
*	Each of the three bits can either be on (1) or off (0).
*/
uint32_t wm_get_mouse_mask(wm_window_t* window);

/**	@brief A function to get the mask for the keyboard keys
*	@param window a pointer to a wm_window_t, which is a previously created window
*	@returns a uint32_t, which represents the 4 bits in the enum in this file
*
*	Get a mask of all keyboard keys currently held.
*	Each of the four bits can either be on (1) or off (0).
*/
uint32_t wm_get_key_mask(wm_window_t* window);

/**	@brief A function to get mouse movement
*	@param window a pointer to a wm_window_t, which is a previously created window
*	@param x a pointer to an integer, which is the x position of the mouse
*	@param y a pointer to an integer, which is the y position of the mouse
*	@returns none
* 
*	Get relative mouse movement in x and y.
*	Pointers to x and y are passed in, so they can both be accessed
*	after the function runs
*/
void wm_get_mouse_move(wm_window_t* window, int* x, int* y);

/**	@brief A function to get the raw OS window object
*	@param window a pointer to a wm_window_t, which is a previously created window
*	@returns none
*	
*	Get the raw OS window object.
*	Returns the HWND of the window object
*/
void* wm_get_raw_window(wm_window_t* window);