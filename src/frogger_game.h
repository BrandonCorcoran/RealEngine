/** @file frogger_game.h
*	@brief A header file for the implementation of a frogger game
* 
*	These set of operations allow for the creation of a frogger game.
*	Major engine systems are brought together, including the heap, file system,
*	window manager, and render system. For the game to run, all that needs to be called
*	is the create function, along with the update function. These two functions are all
*	that is neeeded by the user to implement this game, as all other work is done inside
*	of the simple_game C file. Once the user is ready to terminate the program, the destroy function
*	needs to be called.
*/
#pragma once

/**	@brief A handle to a frogger_game object
*
*	This will hold all relevant information that is needed for the frogger_game.
*	An object of this type must be created in order to have a usable game.
*/
typedef struct frogger_game_t frogger_game_t;

/**	@brief A handle to a file system object
*
*	This will hold all relevant information that is needed for a file system.
*	More about the file system can be found in fs.h. The file system will be used to read
*	in information relevant to the game, such as .vert and .frag files, used to render geometry.
*/
typedef struct fs_t fs_t;

/** @brief A handle to a heap object
*
*	This will hold all relevant information that is needed for the heap.
*	More about the heap can be found in heap.h. The heap will be used
*	for memory allocation and deallocation.
*/
typedef struct heap_t heap_t;

/**	@brief A handle to a render object
*
*	This will hold all relevant information that is needed for rendering objects.
*	More about this system can be found in render.h. The render system is used to
*	update the window and draw entities on the screen. Each object to be rendered is
*	pushed onto a queue to await being processed.
*/
typedef struct render_t render_t;

/**	@brief A handle to a window manager window object
*
*	This will hold all relevant information that is needed for the window
*	that objects will be rendered in. This window is where the game will be
*	displayed
*/
typedef struct wm_window_t wm_window_t;

/**	@brief A function to create a frogger_game object, which is needed to update the game
*	@param heap a pointer to a heap_t object, which is how memory is managed
*	@param fs a pointer to a fs_t object, which is our file system; this is how files will be read into the game (i.e. entity geometry)
*	@param window a pointer to a wm_window_t object, which is the window that all entities will be drawn and rendered in the game
*	@param render a pointer to a render_t object, which is the system that will do the actual drawing / rendering to the window
*	@returns a frogger_game_t pointer, which is used to update the game
*
*	Create an instance of frogger game. This function call is required in order to run the game and allow it to be updated
*/
frogger_game_t* frogger_game_create(heap_t* heap, fs_t* fs, wm_window_t* window, render_t* render);

/**	@brief A function to destroy a frogger_game object
*	@param game a frogger_game_t pointer, which is just the pointer returned from a call to the create function
*	@returns none
*
*	Destroy an instance of frogger game. This function call is necessary to properly deallocate
*	the members of frogger_game.
*/
void frogger_game_destroy(frogger_game_t* game);

/**	@brief A function to update a frogger_game object
*	@param game a frogger_game_t pointer, which is just the pointer returned from a call to the create function
*	@returns none
*
*	Per-frame update for our frogger game. This function can be paired with functions in wm.h to update the
*	game on every pump of the window (and can be added to a while loop). For those more familiar with Unity, this
*	is our equivalent of an Update function, where each call will update the window with the necessary new draws that are ready.
*/
void frogger_game_update(frogger_game_t* game);