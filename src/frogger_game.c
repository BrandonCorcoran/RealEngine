#include "ecs.h"
#include "fs.h"
#include "gpu.h"
#include "heap.h"
#include "render.h"
#include "timer_object.h"
#include "transform.h"
#include "wm.h"
#include "atomic.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>

#include<Windows.h>

typedef struct transform_component_t
{
	transform_t transform;
} transform_component_t;

typedef struct camera_component_t
{
	mat4f_t projection;
	mat4f_t view;
} camera_component_t;

typedef struct model_component_t
{
	gpu_mesh_info_t* mesh_info;
	gpu_shader_info_t* shader_info;
} model_component_t;

typedef struct player_component_t
{
	int index;
} player_component_t;

typedef struct truck_component_t
{
	int index;
	int dir;
} truck_component_t;

typedef struct name_component_t
{
	char name[32];
} name_component_t;

typedef struct frogger_game_t
{
	heap_t* heap;
	fs_t* fs;
	wm_window_t* window;
	HWND window_hwnd;
	render_t* render;

	timer_object_t* timer;

	ecs_t* ecs;
	int transform_type;
	int camera_type;
	int model_type;
	int player_type;
	int truck_type;
	int name_type;
	ecs_entity_ref_t player_ent;
	ecs_entity_ref_t truck_ent;
	ecs_entity_ref_t camera_ent;
	int player_count;
	int truck_count;

	gpu_mesh_info_t cube_mesh_player;
	gpu_mesh_info_t cube_mesh_truck;
	gpu_shader_info_t cube_shader;
	fs_work_t* vertex_shader_work;
	fs_work_t* fragment_shader_work;
} frogger_game_t;

static void load_resources(frogger_game_t* game);
static void unload_resources(frogger_game_t* game);
static void spawn_player(frogger_game_t* game, int index);
static void spawn_truck(frogger_game_t* game, int index);
static void spawn_camera(frogger_game_t* game);
static void update_players(frogger_game_t* game);
static void update_trucks(frogger_game_t* game);
static void draw_models(frogger_game_t* game);

frogger_game_t* frogger_game_create(heap_t* heap, fs_t* fs, wm_window_t* window, render_t* render)
{
	frogger_game_t* game = heap_alloc(heap, sizeof(frogger_game_t), 8);
	game->heap = heap;
	game->fs = fs;
	game->window = window;
	game->render = render;

	game->player_count = 0;
	game->truck_count = 0;

	game->timer = timer_object_create(heap, NULL);

	game->ecs = ecs_create(heap);
	game->transform_type = ecs_register_component_type(game->ecs, "transform", sizeof(transform_component_t), _Alignof(transform_component_t));
	game->camera_type = ecs_register_component_type(game->ecs, "camera", sizeof(camera_component_t), _Alignof(camera_component_t));
	game->model_type = ecs_register_component_type(game->ecs, "model", sizeof(model_component_t), _Alignof(model_component_t));
	game->player_type = ecs_register_component_type(game->ecs, "player", sizeof(player_component_t), _Alignof(player_component_t));
	game->truck_type = ecs_register_component_type(game->ecs, "truck", sizeof(truck_component_t), _Alignof(truck_component_t));
	game->name_type = ecs_register_component_type(game->ecs, "name", sizeof(name_component_t), _Alignof(name_component_t));

	load_resources(game);
	
	spawn_player(game, 0);

	// ROW 1 (Bottom)
	// 0
	spawn_truck(game, 14);
	spawn_truck(game, 28);
	// 1
	spawn_truck(game, 15);
	spawn_truck(game, 29);
	spawn_truck(game, 43);
	// 2
	spawn_truck(game, 16);
	spawn_truck(game, 30);
	spawn_truck(game, 44);
	// 3
	spawn_truck(game, 17);
	spawn_truck(game, 31);
	spawn_truck(game, 45);
	// 4
	spawn_truck(game, 18);
	spawn_truck(game, 32);
	
	// ROW 2
	// 5
	spawn_truck(game, 19);
	spawn_truck(game, 33);
	// 6
	spawn_truck(game, 20);
	spawn_truck(game, 34);
	spawn_truck(game, 48);
	spawn_truck(game, 62);
	// 7
	spawn_truck(game, 21);
	spawn_truck(game, 35);
	spawn_truck(game, 49);
	spawn_truck(game, 63);
	// 8
	spawn_truck(game, 22);
	spawn_truck(game, 36);
	spawn_truck(game, 50);

	// ROW 3
	// 9
	spawn_truck(game, 23);
	spawn_truck(game, 37);
	spawn_truck(game, 51);
	// 10
	spawn_truck(game, 24);
	spawn_truck(game, 38);
	// 11
	spawn_truck(game, 25);
	spawn_truck(game, 39);
	// 12
	spawn_truck(game, 26);
	spawn_truck(game, 40);
	spawn_truck(game, 54);
	// 13
	spawn_truck(game, 27);
	spawn_truck(game, 41);
		
	spawn_camera(game);

	return game;
}

void frogger_game_destroy(frogger_game_t* game)
{
	ecs_destroy(game->ecs);
	timer_object_destroy(game->timer);
	unload_resources(game);
	heap_free(game->heap, game);
}

void frogger_game_update(frogger_game_t* game)
{
	timer_object_update(game->timer);
	ecs_update(game->ecs);
	update_players(game);
	update_trucks(game);
	draw_models(game);
	render_push_done(game->render);
}

static void load_resources(frogger_game_t* game)
{
	game->vertex_shader_work = fs_read(game->fs, "shaders/triangle.vert.spv", game->heap, false, false);
	game->fragment_shader_work = fs_read(game->fs, "shaders/triangle.frag.spv", game->heap, false, false);
	game->cube_shader = (gpu_shader_info_t)
	{
		.vertex_shader_data = fs_work_get_buffer(game->vertex_shader_work),
		.vertex_shader_size = fs_work_get_size(game->vertex_shader_work),
		.fragment_shader_data = fs_work_get_buffer(game->fragment_shader_work),
		.fragment_shader_size = fs_work_get_size(game->fragment_shader_work),
		.uniform_buffer_count = 1,
	};

	// first value is the verts, second value is the color
	static vec3f_t cube_verts_player[] =
	{
		{ -0.25f, -0.25f,  0.25f }, { 0.0f, 0.2f, 0.0f },
		{  0.25f, -0.25f,  0.25f }, { 0.0f, 0.2f, 0.0f },
		{  0.25f,  0.25f,  0.25f }, { 0.0f, 0.2f, 0.0f },
		{ -0.25f,  0.25f,  0.25f }, { 0.0f, 0.2f, 0.0f },
		{ -0.25f, -0.25f, -0.25f }, { 0.0f, 0.2f, 0.0f },
		{  0.25f, -0.25f, -0.25f }, { 0.0f, 0.2f, 0.0f },
		{  0.25f,  0.25f, -0.25f }, { 0.0f, 0.2f, 0.0f },
		{ -0.25f,  0.25f, -0.25f }, { 0.0f, 0.2f, 0.0f },
	};
	static vec3f_t cube_verts_truck[] =
	{
		{ -0.25f, -0.25f,  0.25f }, { 0.5f, 0.5f, 0.5f },
		{  0.25f, -0.25f,  0.25f }, { 0.5f, 0.5f, 0.5f },
		{  0.25f,  0.25f,  0.25f }, { 0.5f, 0.5f, 0.5f },
		{ -0.25f,  0.25f,  0.25f }, { 0.5f, 0.5f, 0.5f },
		{ -0.25f, -0.25f, -0.25f }, { 0.5f, 0.5f, 0.5f },
		{  0.25f, -0.25f, -0.25f }, { 0.5f, 0.5f, 0.5f },
		{  0.25f,  0.25f, -0.25f }, { 0.5f, 0.5f, 0.5f },
		{ -0.25f,  0.25f, -0.25f }, { 0.5f, 0.5f, 0.5f },
	};
	static uint16_t cube_indices[] =
	{
		0, 1, 2,
		2, 3, 0,
		1, 5, 6,
		6, 2, 1,
		7, 6, 5,
		5, 4, 7,
		4, 0, 3,
		3, 7, 4,
		4, 5, 1,
		1, 0, 4,
		3, 2, 6,
		6, 7, 3
	};
	// Allow for different colors for players and trucks
	game->cube_mesh_player = (gpu_mesh_info_t)
	{
		.layout = k_gpu_mesh_layout_tri_p444_c444_i2,
		.vertex_data = cube_verts_player,
		.vertex_data_size = sizeof(cube_verts_player),
		.index_data = cube_indices,
		.index_data_size = sizeof(cube_indices),
	};
	game->cube_mesh_truck = (gpu_mesh_info_t)
	{
		.layout = k_gpu_mesh_layout_tri_p444_c444_i2,
		.vertex_data = cube_verts_truck,
		.vertex_data_size = sizeof(cube_verts_truck),
		.index_data = cube_indices,
		.index_data_size = sizeof(cube_indices),
	};
}

static void unload_resources(frogger_game_t* game)
{
	fs_work_destroy(game->fragment_shader_work);
	fs_work_destroy(game->vertex_shader_work);
}

static void spawn_player(frogger_game_t* game, int index)
{
	game->player_count++;

	uint64_t k_player_ent_mask =
		(1ULL << game->transform_type) |
		(1ULL << game->model_type) |
		(1ULL << game->player_type) |
		(1ULL << game->name_type);
	game->player_ent = ecs_entity_add(game->ecs, k_player_ent_mask, index);

	transform_component_t* transform_comp = ecs_entity_get_component(game->ecs, game->player_ent, game->transform_type, true);
	transform_identity(&transform_comp->transform);

	transform_comp->transform.translation.z = 3.75f;

	name_component_t* name_comp = ecs_entity_get_component(game->ecs, game->player_ent, game->name_type, true);
	strcpy_s(name_comp->name, sizeof(name_comp->name), "player");

	player_component_t* player_comp = ecs_entity_get_component(game->ecs, game->player_ent, game->player_type, true);
	player_comp->index = index;

	model_component_t* model_comp = ecs_entity_get_component(game->ecs, game->player_ent, game->model_type, true);
	model_comp->mesh_info = &game->cube_mesh_player;
	model_comp->shader_info = &game->cube_shader;
}

static void spawn_truck(frogger_game_t* game, int index)
{
	game->truck_count++;

	uint64_t k_truck_ent_mask =
		(1ULL << game->transform_type) |
		(1ULL << game->model_type) |
		(1ULL << game->truck_type) |
		(1ULL << game->name_type);
	game->truck_ent = ecs_entity_add(game->ecs, k_truck_ent_mask, index);

	transform_component_t* transform_comp = ecs_entity_get_component(game->ecs, game->truck_ent, game->transform_type, true);
	transform_identity(&transform_comp->transform);
	// Space out trucks and give them their initial placement
	float dist_between = 0.5f;
	int num_trucks = 14;
	// ROW 1 (Bottom)
	if (index % num_trucks == 0)			// modulus allows us to break these out on a per-truck basis
	{
		transform_comp->transform.translation.y = -6.5f + (index/ num_trucks * dist_between);		// (index / num_trucks) properly spaces out the individual squares (as in 1*dist_between, 2*dist_between)
		transform_comp->transform.translation.z = 2.5f;
	}
	else if (index % num_trucks == 1)
	{
		transform_comp->transform.translation.y = -3.5f + (index/ num_trucks * dist_between);
		transform_comp->transform.translation.z = 2.5f;
	}
	else if (index % num_trucks == 2)
	{
		transform_comp->transform.translation.y = 0.0f + (index/ num_trucks * dist_between);
		transform_comp->transform.translation.z = 2.5f;
	}
	else if (index % num_trucks == 3)
	{
		transform_comp->transform.translation.y = 3.5f + (index / num_trucks * dist_between);
		transform_comp->transform.translation.z = 2.5f;
	}
	else if (index % num_trucks == 4)
	{
		transform_comp->transform.translation.y = 6.5f + (index / num_trucks * dist_between);
		transform_comp->transform.translation.z = 2.5f;
	}
	// ROW 2
	else if (index % num_trucks == 5)
	{
		transform_comp->transform.translation.y = -7.0f + (index/ num_trucks * dist_between);
		transform_comp->transform.translation.z = 0.0f;
	}
	else if (index % num_trucks == 6)
	{
		transform_comp->transform.translation.y = -4.0f + (index/ num_trucks * dist_between);
		transform_comp->transform.translation.z = 0.0f;
	}
	else if (index % num_trucks == 7)
	{
		transform_comp->transform.translation.y = 0.5f + (index / num_trucks * dist_between);
		transform_comp->transform.translation.z = 0.0f;
	}
	else if (index % num_trucks == 8)
	{
		transform_comp->transform.translation.y = 6.5f + (index / num_trucks * dist_between);
		transform_comp->transform.translation.z = 0.0f;
	}
	// ROW 3 (Top)
	else if (index % num_trucks == 9)
	{
		transform_comp->transform.translation.y = -6.0f + (index/ num_trucks * dist_between);
		transform_comp->transform.translation.z = -2.5f;
	}
	else if (index % num_trucks == 10)
	{
		transform_comp->transform.translation.y = -3.0f + (index/ num_trucks * dist_between);
		transform_comp->transform.translation.z = -2.5f;
	}
	else if (index % num_trucks == 11)
	{
		transform_comp->transform.translation.y = -0.5f + (index / num_trucks * dist_between);
		transform_comp->transform.translation.z = -2.5f;
	}
	else if (index % num_trucks == 12)
	{
		transform_comp->transform.translation.y = 4.0f + (index / num_trucks * dist_between);
		transform_comp->transform.translation.z = -2.5f;
	}
	else if (index % num_trucks == 13)
	{
		transform_comp->transform.translation.y = 7.5f + (index / num_trucks * dist_between);
		transform_comp->transform.translation.z = -2.5f;
	}

	name_component_t* name_comp = ecs_entity_get_component(game->ecs, game->truck_ent, game->name_type, true);
	strcpy_s(name_comp->name, sizeof(name_comp->name), "truck");

	truck_component_t* truck_comp = ecs_entity_get_component(game->ecs, game->truck_ent, game->truck_type, true);
	truck_comp->index = index;
	// Set direction depending on the lane
	if (index % num_trucks >= 5 && index % num_trucks <= 8)
	{
		truck_comp->dir = 0;
	}
	else
	{
		truck_comp->dir = 1;
	}

	model_component_t* model_comp = ecs_entity_get_component(game->ecs, game->truck_ent, game->model_type, true);
	model_comp->mesh_info = &game->cube_mesh_truck;
	model_comp->shader_info = &game->cube_shader;
}

static void spawn_camera(frogger_game_t* game)
{
	uint64_t k_camera_ent_mask =
		(1ULL << game->camera_type) |
		(1ULL << game->name_type);
	game->camera_ent = ecs_entity_add(game->ecs, k_camera_ent_mask, 0);

	name_component_t* name_comp = ecs_entity_get_component(game->ecs, game->camera_ent, game->name_type, true);
	strcpy_s(name_comp->name, sizeof(name_comp->name), "camera");

	camera_component_t* camera_comp = ecs_entity_get_component(game->ecs, game->camera_ent, game->camera_type, true);
	
	// We use these dimensions given that the screen is 16:9 ; (-4, 4) is used because the squares are 0.5 in length
	mat4f_make_orthographic(&camera_comp->projection, -8.0f, 8.0f, 4.0f, -4.0f, 0.1f, 100.0f);

	vec3f_t eye_pos = vec3f_scale(vec3f_forward(), -5.0f);
	vec3f_t forward = vec3f_forward();
	vec3f_t up = vec3f_up();
	mat4f_make_lookat(&camera_comp->view, &eye_pos, &forward, &up);
}

static void update_players(frogger_game_t* game)
{
	float dt = (float)timer_object_get_delta_ms(game->timer) * 0.001f;

	uint32_t key_mask = wm_get_key_mask(game->window);

	uint64_t k_query_mask = (1ULL << game->transform_type) | (1ULL << game->player_type);

	for (ecs_query_t query = ecs_query_create(game->ecs, k_query_mask);
		ecs_query_is_valid(game->ecs, &query);
		ecs_query_next(game->ecs, &query))
	{
		transform_component_t* transform_comp = ecs_query_get_component(game->ecs, &query, game->transform_type);
		player_component_t* player_comp = ecs_query_get_component(game->ecs, &query, game->player_type);

		transform_t move;
		transform_identity(&move);
		if (key_mask & k_key_up)
		{
			move.translation = vec3f_add(move.translation, vec3f_scale(vec3f_up(), -dt));
		}
		if (key_mask & k_key_down)
		{
			move.translation = vec3f_add(move.translation, vec3f_scale(vec3f_up(), dt));
		}
		if (key_mask & k_key_left)
		{
			move.translation = vec3f_add(move.translation, vec3f_scale(vec3f_right(), -dt));
		}
		if (key_mask & k_key_right)
		{
			move.translation = vec3f_add(move.translation, vec3f_scale(vec3f_right(), dt));
		}
		transform_multiply(&transform_comp->transform, &move);

		// Reset positions on outer bounds of the window
		if (transform_comp->transform.translation.z > 3.75f)
		{
			transform_comp->transform.translation.z = 3.75f;
		}
		else if (transform_comp->transform.translation.z < -3.75f)
		{
			transform_comp->transform.translation.z = 3.75f;
			transform_comp->transform.translation.y = 0.0f;
		}
		if (transform_comp->transform.translation.y > 7.75f)
		{
			transform_comp->transform.translation.y = 7.75f;
		}
		else if (transform_comp->transform.translation.y < -7.75f)
		{
			transform_comp->transform.translation.y = -7.75f;
		}

	}
}

// automatically move trucks accordingly
static void update_trucks(frogger_game_t* game)
{
	// truck speed
	float dt = (float)timer_object_get_delta_ms(game->timer) * 0.00025f;

	uint32_t key_mask = wm_get_key_mask(game->window);

	uint64_t k_query_mask = (1ULL << game->transform_type) | (1ULL << game->truck_type);

	for (ecs_query_t query = ecs_query_create(game->ecs, k_query_mask);
		ecs_query_is_valid(game->ecs, &query);
		ecs_query_next(game->ecs, &query))
	{
		transform_component_t* transform_comp = ecs_query_get_component(game->ecs, &query, game->transform_type);
		truck_component_t* truck_comp = ecs_query_get_component(game->ecs, &query, game->truck_type);

		transform_t move;
		transform_identity(&move);
		if (truck_comp->dir == 1)
		{
			// LEFT
			move.translation = vec3f_add(move.translation, vec3f_scale(vec3f_right(), -dt));
		}
		else
		{
			// RIGHT
			move.translation = vec3f_add(move.translation, vec3f_scale(vec3f_right(), dt));
		}
		
		transform_multiply(&transform_comp->transform, &move);

		if (transform_comp->transform.translation.y < -9.0f)
		{
			transform_comp->transform.translation.y = 8.25f;
		}
		else if (transform_comp->transform.translation.y > 9.0f)
		{
			transform_comp->transform.translation.y = -8.25f;
		}

		uint64_t k_query_mask_player = (1ULL << game->transform_type) | (1ULL << game->player_type);
		ecs_query_t query_player = ecs_query_create(game->ecs, k_query_mask_player);
		transform_component_t* transform_comp_player = ecs_query_get_component(game->ecs, &query_player, game->transform_type);

		// Calculate collisions between trucks and players
		// We do this in update_trucks() because otherwise we would have to loop through every truck in update_player()
		float player_y = transform_comp_player->transform.translation.y;
		float truck_y = transform_comp->transform.translation.y;
		if ( (player_y - 0.25f >= truck_y - 0.25f && player_y - 0.25f <= truck_y + 0.25f) || (player_y + 0.25f >= truck_y - 0.25f && player_y + 0.25f <= truck_y + 0.25f) )
		{
			float player_z = transform_comp_player->transform.translation.z;
			float truck_z = transform_comp->transform.translation.z;
			if ( (player_z-0.25f >= truck_z-0.25f && player_z-0.25f <= truck_z+0.25f) || (player_z+0.25f >= truck_z-0.25f && player_z+0.25f <= truck_z+0.25f) )
			{
				transform_comp_player->transform.translation.z = 3.75f;
				transform_comp_player->transform.translation.y = 0.0f;
			}
		}

	}
}

static void draw_models(frogger_game_t* game)
{
	uint64_t k_camera_query_mask = (1ULL << game->camera_type);
	for (ecs_query_t camera_query = ecs_query_create(game->ecs, k_camera_query_mask);
		ecs_query_is_valid(game->ecs, &camera_query);
		ecs_query_next(game->ecs, &camera_query))
	{
		camera_component_t* camera_comp = ecs_query_get_component(game->ecs, &camera_query, game->camera_type);

		uint64_t k_model_query_mask = (1ULL << game->transform_type) | (1ULL << game->model_type);
		for (ecs_query_t query = ecs_query_create(game->ecs, k_model_query_mask);
			ecs_query_is_valid(game->ecs, &query);
			ecs_query_next(game->ecs, &query))
		{
			transform_component_t* transform_comp = ecs_query_get_component(game->ecs, &query, game->transform_type);
			model_component_t* model_comp = ecs_query_get_component(game->ecs, &query, game->model_type);
			ecs_entity_ref_t entity_ref = ecs_query_get_entity(game->ecs, &query);

			struct
			{
				mat4f_t projection;
				mat4f_t model;
				mat4f_t view;
			} uniform_data;
			uniform_data.projection = camera_comp->projection;
			uniform_data.view = camera_comp->view;
			transform_to_matrix(&transform_comp->transform, &uniform_data.model);
			gpu_uniform_buffer_info_t uniform_info = { .data = &uniform_data, sizeof(uniform_data) };

			render_push_model(game->render, &entity_ref, model_comp->mesh_info, model_comp->shader_info, &uniform_info);
		}
	}
}