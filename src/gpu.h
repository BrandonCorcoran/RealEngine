/** @file gpu.h
*	@brief A header file for the implementation of a Vulkan GPU
* 
*	This set of operations allows for the creation of a Vulkan GPU.
*	This GPU is able to hold meshes, shaders and also works 
*	to draw frames to the window. This is accomplished
*	by implementing Vulkan and working with its functions
*/
#pragma once

#include <stdbool.h>

/**	@brief A handle to a Vulkan GPU object.
* 
*	This will hold our Vulkan instance to be used with
*	the following functions.
*/
typedef struct gpu_t gpu_t;

/**	@brief A handle to a Vulkan buffer object for the GPU.
*
*	This holds a Vulkan command buffer and
*	pipeline layout. It also keeps
*	track of the index and vertex counts.
*/
typedef struct gpu_cmd_buffer_t gpu_cmd_buffer_t;

/**	@brief A handle to a Vulkan descriptor for the GPU.
*
*	This holds a Vulkan descriptor set.
*/
typedef struct gpu_descriptor_t gpu_descriptor_t;

/**	@brief A handle to a Vulkan mesh for the GPU.
*
*	This holds a buffer and index / vertex data,
*	which is necessary for being able to use
*	meshes.
*/
typedef struct gpu_mesh_t gpu_mesh_t;

/**	@brief A handle to a Vulkan pipeline for the GPU.
*
*	This holds a Vulkan pipeline layout as well
*	as a Vulkan pipeline itself.
*/
typedef struct gpu_pipeline_t gpu_pipeline_t;

/**	@brief A handle to a Vulkan shader for the GPU.
*
*	This holds Vulkan shaders, which are for
*	vertex and fragment modules.
*/
typedef struct gpu_shader_t gpu_shader_t;

/**	@brief A handle to a Vulkan uniform buffer for the GPU.
*
*	This contains a Vulkan buffer as well as a Vulkan
*	object that holds device memory.
*/
typedef struct gpu_uniform_buffer_t gpu_uniform_buffer_t;

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

/**	@brief A handle to a GPU descriptor
*
*	This holds information related to a shader
*	and also has multiple uniform buffers (a
*	2D representation of them)
*/
typedef struct gpu_descriptor_info_t
{
	gpu_shader_t* shader;
	gpu_uniform_buffer_t** uniform_buffers;
	int uniform_buffer_count;
} gpu_descriptor_info_t;

/**	@brief A handle to a GPU mesh layout
*
*	This holds pre-created mesh layouts
*	as well as a count of the number of 
*	mesh layouts
*/
typedef enum gpu_mesh_layout_t
{
	k_gpu_mesh_layout_tri_p444_i2,
	k_gpu_mesh_layout_tri_p444_c444_i2,

	k_gpu_mesh_layout_count,
} gpu_mesh_layout_t;

/**	@brief A handle to mesh info for the GPU
*
*	This handle contains data for vertex data as
*	well as index data that is used for a mesh
*/
typedef struct gpu_mesh_info_t
{
	gpu_mesh_layout_t layout;
	void* vertex_data;
	size_t vertex_data_size;
	void* index_data;
	size_t index_data_size;
} gpu_mesh_info_t;

/**	@brief A handle to pipeline information for the GPU
*
*	This holds shader information as well as
*	a mesh layout
*/
typedef struct gpu_pipeline_info_t
{
	gpu_shader_t* shader;
	gpu_mesh_layout_t mesh_layout;
} gpu_pipeline_info_t;

/**	@brief A handle to shader info for the GPU
*
*	This handle contains data for vertex shader
*	as well as fragment shader.
*/
typedef struct gpu_shader_info_t
{
	void* vertex_shader_data;
	size_t vertex_shader_size;
	void* fragment_shader_data;
	size_t fragment_shader_size;
	int uniform_buffer_count;
} gpu_shader_info_t;

/**	@brief A handle for the uniform buffer of the GPU
*
*	This handle holds the GPU buffer
*	data and size
*/
typedef struct gpu_uniform_buffer_info_t
{
	void* data;
	size_t size;
} gpu_uniform_buffer_info_t;

/**	@brief A function to create a GPU object
*	@param heap a pointer to a heap_t object, which is how memory is managed
*	@param window a pointer to a wm_window_t object, which is the window that all entities will be drawn and rendered in the game
*	@returns a gpu_t pointer, which is a GPU object that is an insance of Vulkan
*
*	This function creates an instance of Vulkan on the 
*	provided window
*/
gpu_t* gpu_create(heap_t* heap, wm_window_t* window);

/**	@brief A function to destroy a GPU object
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@returns none
*
*	This function destroys a previously created Vulkan, which
*	is necessary to prevent any memory leaks from allocated
*	memory
*/
void gpu_destroy(gpu_t* gpu);

/**	@brief A function to get the frame count in the swap chain
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@returns an integer, which represents the current frame count
*
*	This function gets the number of frames in the swapchain
*/
int gpu_get_frame_count(gpu_t* gpu);

/**	@brief A function to wait for the GPU to finish its queued work
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@returns none
*
*	This function waits for the GPU to be done with all queued work.
*	The GPU will idle during this time
*/
void gpu_wait_until_idle(gpu_t* gpu);

/**	@brief A function to bind uniform buffers to shaders
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param info a const gpu_descriptor_info_t pointer, which holds shader and buffer information.
*	@returns a gpu_descriptor_t pointer, which is the newly created GPU descriptor (created through Vulkan functions)
*
*	This function binds uniform buffers
*	(and textures if we had them) to a given shader layout.
*	This is accomplished through Vulkan functions.
*/
gpu_descriptor_t* gpu_descriptor_create(gpu_t* gpu, const gpu_descriptor_info_t* info);

/**	@brief A function to destroy a descriptor
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param descriptor a gpu_descriptor_t pointer, which is a previously created descriptor
*	@returns none
*
*	This function destroys a previously created descriptor
*	which is necessary to prevent memory leaks from previously allocated
*	memory used to create the descriptor.
*/
void gpu_descriptor_destroy(gpu_t* gpu, gpu_descriptor_t* descriptor);

/**	@brief A function to create drawable geometry.
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param info a const gpu_mesh_info_t pointer, which holds vertex and index data needed to create a mesh.
*	@returns a gpu_mesh_t pointer, which is a Vulkan mesh holding the same information.
*
*	This function creates a drawable piece of geometry with vertex and index data.
*	This is accomplished by reading in vertex and index data and then
*	using it to create this new mesh object.
*/
gpu_mesh_t* gpu_mesh_create(gpu_t* gpu, const gpu_mesh_info_t* info);

/**	@brief A function to destroy some geometry
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param mesh a gpu_mesh_t pointer, which is a previously created mesh
*	@returns none
* 
*	This function destroys some previously created geometry and is
*	necessary to call to properly deallocate memory.
*/
void gpu_mesh_destroy(gpu_t* gpu, gpu_mesh_t* mesh);

/**	@brief A function to create an object with the ability to bind.
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param info a const gpu_pipeline_info_t pointer, which holds the informatio necessary to createa a pipeline
*	@returns a gpu_pipeline_t pointer, which holds a shader and mesh layout
*
*	This function sets up an object that binds a 
*	shader to a mesh layout for rendering. This is accomplished 
*	through Vulkan functions, which allow for the creation and initial
*	set up of this pipeline.
*/
gpu_pipeline_t* gpu_pipeline_create(gpu_t* gpu, const gpu_pipeline_info_t* info);

/**	@brief A function to destroy a pipeline.
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param pipeline a gpu_pipeline_t pointer, which is a previously created pipeline
*	@returns none
* 
*	This function destroys a pipeline. This function call is necessary
*	to properly deallocate any allocated memory.
*/
void gpu_pipeline_destroy(gpu_t* gpu, gpu_pipeline_t* pipeline);

/**	@brief A function to create a shader object.
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param info a const gpu_shader_info_t pointer, which is the data necessary to create a shader
*	@returns a gpu_shader_t pointer, which is the shader object
*
*	This function creates a shader object with vertex and fragment shader programs.
*	This is accomplished by using Vulkan functions.
*/
gpu_shader_t* gpu_shader_create(gpu_t* gpu, const gpu_shader_info_t* info);

/**	@brief A function to destroy a shader
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param shader a gpu_shader_t pointer, which is a previously created shader object.
*	@returns none
*
*	This function destroys a shader and is necessary
*	to call to properly deallocate allocated memory.
*/
void gpu_shader_destroy(gpu_t* gpu, gpu_shader_t* shader);

/**	@brief A function to create a uniform buffer
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param info a const gpu_uniform_buffer_info_t pointer, which holds data and its size.
*	@returns a gpu_uniform_buffer_t, which holds Vulkan buffer, memory, and descriptor
*	
*	This function creates a uniform buffer with specified size and contents.
*	The buffer will be consumed by a shader.
*/
gpu_uniform_buffer_t* gpu_uniform_buffer_create(gpu_t* gpu, const gpu_uniform_buffer_info_t* info);

/**	@brief A function to update a uniform buffer
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param buffer a gpu_uniform_buffer_t pointer, which holds the data in a previously created uniform buffer
*	@param data a const void pointer, which holds data to be stored in the buffer
*	@param size a size_t, which holds the size of the passed in data
*	@returns none
* 
*	This function modifies an existing uniform buffer. Memory passed in
*	through data is copied to a void* and mapped via Vulkan. Memory in the
*	buffer is also unmapped.
*/
void gpu_uniform_buffer_update(gpu_t* gpu, gpu_uniform_buffer_t* buffer, const void* data, size_t size);

/**	@brief A function to destroy a uniform buffer
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param buffer a gpu_uniform_buffer_t pointer, which is a previously created uniform buffer
*	@returns none
* 
*	This function destroys a previously created uniform buffer.
*	This is necessary to deallocate any previously allocated memory
*	used to create the uniform buffer.
*/
void gpu_uniform_buffer_destroy(gpu_t* gpu, gpu_uniform_buffer_t* buffer);

/**	@brief A function to start a new frame of recording
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@returns a gpu_cmd_buffer_t pointer, which holds a buffer, pipeline information and counts of indices / vertices
*
*	This function starts a new frame of rendering.
*	It may wait on a prior frame to complete. 
*	The function returns a command buffer for all rendering in that frame.
*/
gpu_cmd_buffer_t* gpu_frame_begin(gpu_t* gpu);

/**	@brief A function to finish rendering a frame.
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@returns none
*
*	This function finishes rendering a frame. Vulkan
*	functions are relied on to end the render pass
*	and acquire subsequent images.
*/
void gpu_frame_end(gpu_t* gpu);

/**	@brief A function to bind a pipeline to a buffer
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param cmd_buffer a gpu_cmd_buffer_t pointer, which holds a buffer, pipeline information and counts of indices / vertices
*	@param pipeline a gpu_pipeline_t pointer, which holds the pipeline layout and a Vulkan pipe.
*	@returns none
*
*	This function sets the current pipeline
*	for the given command buffer. As a result,
*	the command buffer stores the pipeline's
*	layout.
*/
void gpu_cmd_pipeline_bind(gpu_t* gpu, gpu_cmd_buffer_t* cmd_buffer, gpu_pipeline_t* pipeline);

/**	@brief A function to set a command buffer's mesh
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param cmd_buffer a gpu_cmd_buffer_t pointer, which holds a buffer, pipeline information and counts of indices / vertices
*	@param mesh a gpu_mesh_t pointer, which holds index and vertex data for a mesh
*	@returns none
*
*	This function sets the current mesh for this command
*	buffer. This is accomplished throuh a combination of
*	Vulkan functions as well as checking the vertex count
*	and index count.
*/
void gpu_cmd_mesh_bind(gpu_t* gpu, gpu_cmd_buffer_t* cmd_buffer, gpu_mesh_t* mesh);

/**	@brief A function to set a command buffer's descriptor
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param cmd_buffer a gpu_cmd_buffer_t pointer, which holds a buffer, pipeline information and counts of indices / vertices
*	@param descriptor a gpu_descriptor_t pointer, which holds a Vulkan descriptor set
*	@returns none
*
*	This function sets the current descriptor for
*	the given command buffer using Vulkan functions.
*/
void gpu_cmd_descriptor_bind(gpu_t* gpu, gpu_cmd_buffer_t* cmd_buffer, gpu_descriptor_t* descriptor);

/**	@brief A function to draw information in the command buffer
*	@param gpu a gpu_t pointer, which is a previously created instance of Vulkan held in this GPU object
*	@param cmd_buffer a gpu_cmd_buffer_t pointer, which holds a buffer, pipeline information and counts of indices / vertices
*	@returns none
*
*	This function draws the given current pipeline,
*	mesh, and descriptor. This is done through 
*	checking either the index count or the vertext
*	count and using Vulkan functions to draw accordingly.
*/
void gpu_cmd_draw(gpu_t* gpu, gpu_cmd_buffer_t* cmd_buffer);