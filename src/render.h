/**	@file render.h
*	@brief A header file for the implementation of a high-level graphics rendering interface
* 
*	Operations for creating and destroying a render object are supported as well as
*	a function to push a model onto a queue of items to be rendered. Once all desired models
*	are pushed to the queue an end-of-frame marker can be passed in
*	signifying the items in the queue are ready to be rendered
*/
#pragma once

/**	@brief A handle to a render object
*
*	This will hold all relevant information that is needed for rendering objects.
*	The render system is used to
*	update the window and draw entities on the screen. Each object to be rendered is
*	pushed onto a queue to await being processed.
*/
typedef struct render_t render_t;

/** @brief A weak reference to an entity.
*
*	This typedef is a type for a references to an entity in our ecs.
*	This is useful for returning a reference to an entity
*/
typedef struct ecs_entity_ref_t ecs_entity_ref_t;

/**	@brief A handle to mesh info for the GPU
*	
*	This handle contains data for vertex data as
*	well as index data that is used for a mesh
*/
typedef struct gpu_mesh_info_t gpu_mesh_info_t;

/**	@brief A handle to shader info for the GPU
* 
*	This handle contains data for vertex shader 
*	as well as fragment shader.
*/
typedef struct gpu_shader_info_t gpu_shader_info_t;

/**	@brief A handle for the uniform buffer of the GPU
* 
*	This handle holds the GPU buffer 
*	data and size
*/
typedef struct gpu_uniform_buffer_info_t gpu_uniform_buffer_info_t;

/** @brief A handle to a heap object
*
*	This will hold all relevant information that is needed for the heap.
*	More about the heap can be found in heap.h. The heap will be used
*	for memory allocation and deallocation.
*/
typedef struct heap_t heap_t;

/**	@brief A handle to a window manager window object
*
*	This will hold all relevant information that is needed for the window
*	that objects will be rendered in. This window is where the game will be
*	displayed
*/
typedef struct wm_window_t wm_window_t;

/**	@brief A function to create a render system
*	@param heap a pointer to a heap_t object, which is how memory is managed
*	@param window a pointer to a wm_window_t object, which is the window that all entities will be drawn and rendered in the game
*	@returns a render_t pointer, which will be used to contain all of the models we want in the render queue
* 
*	This render system will need to be destroyed before program termination and is used to 
*	push models to the queue and push an end-of-frame marker
*/
render_t* render_create(heap_t* heap, wm_window_t* window);

/**	@brief A function to destroy a render system
*	@param render a render_t pointer, which is the previously created render system.
*	@returns none
* 
*	Destroy a render system. This is necessary to clean up any allocated memory.
*	Failure to do so may result in memory leaks
*/
void render_destroy(render_t* render);

/**	@brief A function to push a model to the render queue
*	@param render a pointer to a render_t object, which is the system that will keep track of which models are to be rendered
*	@param entity a pointer to an ecs_entity_ref_t, which is a reference to an entity
*	@param mesh a pointer to a gpu_mesh_info_t, which is a handle to mesh info for the GPU (to be used with that entity)
*	@param shader a pointer to a gpu_shader_info_t, which is a handle to shader info for the GPU (to be used with that entity)
*	@param uniform a pointer to a uniform_buffer_info_t, which holds information to the uniform buffer of the GPU
*	@returns none
* 
*	This function pushes a model onto the queue of items in render. This model is now waiting
*	to be rendered. The model has a mesh and shader that will be attached to it in the GPU.
*/
void render_push_model(render_t* render, ecs_entity_ref_t* entity, gpu_mesh_info_t* mesh, gpu_shader_info_t* shader, gpu_uniform_buffer_info_t* uniform);

/**	@brief A function to tell the render queue the items are ready to be rendered
*	@param render a pointer to a render_t object, which is the system that will keep track of which models are to be rendered
*	@returns none
* 
*	This function pushes an end-of-frame marker on a queue of items to be rendered.
*	The items in the queue are now able to be rendered.
*/
void render_push_done(render_t* render);