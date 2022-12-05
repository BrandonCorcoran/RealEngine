/**	@file simple_game.h
*	@brief A header file for the implementation of a simple "game"
* 
*	This set of operations allow for the creation of a simple "game".
*	Major engine systems are brought together, including the heap, file system,
*	window manager, and render system. For the game to run, all that needs to be called
*	is the create function, along with the update function. These two functions are all
*	that is neeeded by the user to implement this game, as all other work is done inside
*	of the simple_game C file. Once the user is ready to terminate the program, the destroy function
*	needs to be called.
*/
#pragma once

/**	@brief A handle to a simple_game object
* 
*	This will hold all relevant information that is needed for the simple_game.
*	An object of this type must be created in order to have a usable game.
*/
typedef struct simple_game_t simple_game_t;

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

/**	@brief A function to create a simple_game object, which is needed to update the game
*	@param heap a pointer to a heap_t object, which is how memory is managed
*	@param fs a pointer to a fs_t object, which is our file system; this is how files will be read into the game (i.e. entity geometry)
*	@param window a pointer to a wm_window_t object, which is the window that all entities will be drawn and rendered in the game
*	@param render a pointer to a render_t object, which is the system that will keep track of which models are to be rendered
*	@param argc an integer, which is the number of arguments passed in (i.e. the number of arguments in argv)
*	@param argv a const char**, which uses parameters generally passed in through main. While required to pass in, argv can technically be left as-is, along with argc being one. If, however, a user would like to have multiple players in their game, this argument is used for server connections
*	@returns a simple_game_t pointer, which is used to update the game
* 
*	Create an instance of simple test game. This function call is required in order to run the game and allow it to be updated
*/
simple_game_t* simple_game_create(heap_t* heap, fs_t* fs, wm_window_t* window, render_t* render, int argc, const char** argv);

/**	@brief A function to destroy a simple_game object
*	@param game a simple_game_t pointer, which is just the pointer returned from a call to the create function
*	@returns none
*	
*	Destroy an instance of simple test game. This function call is necessary to properly deallocate
*	the members of simple_game.
*/
void simple_game_destroy(simple_game_t* game);

/**	@brief A function to update a simple_game object
*	@param game a simple_game_t pointer, which is just the pointer returned from a call to the create function
*	@returns none
* 
*	Per-frame update for our simple test game. This function can be paired with functions in wm.h to update the
*	game on every pump of the window (and can be added to a while loop). For those more familiar with Unity, this
*	is our equivalent of an Update function, where each call will update the window with the necessary new draws that are ready.
*/
void simple_game_update(simple_game_t* game);