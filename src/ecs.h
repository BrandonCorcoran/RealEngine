/** @file ecs.h
*	@brief A header file for an entity component system
* 
*	This set of functions provides a framework for game entities and their components.
*	The ecs system must first be created (and destroyed before the program finishes).
*	Entities can then be added and removed through this system and components on entities
*	can be registered (and used across other entities). The validity of entities can also be checked
*	along with querying for specific entities.
*/
#pragma once

#include <stdbool.h>
#include <stdint.h>

/** @brief Handle to a heap object
*
*	This typedef is a type for a heap object that works alongside our memory management in heap.h. This helps
*	to contain all the information related to memory and what is being stored on the heap.
*/
typedef struct heap_t heap_t;

/** @brief Handle to an entity component system interface.
*
*	This typedef is a type for the entity component system, which helps to contain all variables related to the ecs.
*/
typedef struct ecs_t ecs_t;

/** @brief A weak reference to an entity.
*
*	This typedef is a type for a references to an entity in our ecs.
*	This is useful for returning a reference to an entity
*/
typedef struct ecs_entity_ref_t
{
	int entity;
	int sequence;
} ecs_entity_ref_t;

/** @brief Working data for an active entity query.
*
*	This typedef is a type that is used to test if an entity matching the query exists.
*	It is used to see if the query is valid.
*/
typedef struct ecs_query_t
{
	uint64_t component_mask;
	int entity;
} ecs_query_t;

/** @brief A function to create an entity component system
*	@param heap a heap_t pointer, which holds all relevant information for a previously-created heap (needed for memory management)
*	@returns an ecs_t pointer, which is a pointer to the component system (this is necessary to pass into other related functions
*	as it will hold all information relevant to the entities and components that are created)
* 
*	Create an entity component system. This function performs the necessary set-up for an ecs object. It is required if
*	a user would like to create entities / components.
*/
ecs_t* ecs_create(heap_t* heap);

/** @brief A function to destroy an entity component system
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@returns none
* 
*	Destroy an entity component system. This function performs the necessary clean-up for an ecs object.
*	This must be done before the program ends in order to properly free memory and ensure all entities are 
*	safely removed.
*/
void ecs_destroy(ecs_t* ecs);

/**	@brief A function to update entities
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@returns none
* 
*	Per-frame entity component system update. This loops through
*	all entities and checks for any entities that are pending
*	add or removal
*/
void ecs_update(ecs_t* ecs);

/**	@brief A function to register a component type
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param name a const char*, which is the name of the component type
*	@param size_per_component a size_t, which is the size of the component type
*	@param alignment a size_t, which is how the component type is byte aligned
*	@returns an integer, which is the index for where this component type is registered (-1 on failure)
* 
*	Register a type of component with the entity system. This loops through
*	all components in ecs and finds an empty spot (if available) to add this component
*	type and copy all information over
*/
int ecs_register_component_type(ecs_t* ecs, const char* name, size_t size_per_component, size_t alignment);

/**	@brief A function to get the size of a registered component
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param component_type an integer, which is the index of where the component is stored
*	@returns a size_t, which represents the size of the component
* 
*	Return the size of a type of component registered with the sytem.
*/
size_t ecs_get_component_type_size(ecs_t* ecs, int component_type);

/**	@brief A function to spawn an entity
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param component_mask a uint64_t, which is a mask for the entity (detailing which component(s) it has)
*	@param index an integer, which is the index of the entity
*	@returns an ecs_entity_ref_t, which is a reference to an entity object
* 
*	This function spawns an entity with the masked components and returns a reference to it.
*/
ecs_entity_ref_t ecs_entity_add(ecs_t* ecs, uint64_t component_mask, int index);

/**	@brief A function to remove an entity
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param ref an ecs_entity_ref_t, which is a reference to an entity object
*	@param allow_pending_add a boolean, which allows for the destruction of an entity that is not fully spawned (if true)
*	@returns an integer, which is the index of the entity set to be removed
* 
*	This function destroys an entity. The entity can either be fully spawned,
*	or not fully spawned.
*/
int ecs_entity_remove(ecs_t* ecs, ecs_entity_ref_t ref, bool allow_pending_add);

/**	@brief A function to check if a reference to an entity is valid
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param ref an ecs_entity_ref_t, which is a reference to an entity object
*	@param allow_pending_add a boolean, which allows for the destruction of an entity that is not fully spawned (if true)
*	@returns a boolean, which returns if the entity reference is to a valid entry
* 
*	Determines if a entity reference points to a valid entity.
*/
bool ecs_is_entity_ref_valid(ecs_t* ecs, ecs_entity_ref_t ref, bool allow_pending_add);

/**	@brief A function to get the section of memory associated with an entity's component
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param ref an ecs_entity_ref_t, which is a reference to an entity object
*	@param component_type an integer, which is the index for the component
*	@param allow_pending_add a boolean, which allows for the destruction of an entity that is not fully spawned (if true)
*	@returns a void pointer, which is the component associated with the provided entity reference
*	
*	Get the memory for a component on an entity.
*	NULL is returned if the entity is not valid or the component_type is not present on the entity.
*/
void* ecs_entity_get_component(ecs_t* ecs, ecs_entity_ref_t ref, int component_type, bool allow_pending_add);

/**	@brief A function to create a new entity query
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param mask a uint64_t, which is a component_type mask that is assigned an entity number / index
*	@returns an ecs_query_t, which is an entity query
* 
*	Creates a new entity query by component type mask.
*	All component masks are looped through to assign the index of an entity
*/
ecs_query_t ecs_query_create(ecs_t* ecs, uint64_t mask);

/**	@brief A function that checks if a query points to a valid entity
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param query an ecs_query_t pointer, which points to a previously created query object
*	@returns a boolean which represents if the query points to a valid entity
* 
*	This function determines if the query points
*	at a valid entity.
*/
bool ecs_query_is_valid(ecs_t* ecs, ecs_query_t* query);

/**	@brief A function that advances the query to the next matching entity
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param query an ecs_query_t pointer, which points to a previously created query object
*	@returns none
*
*	This function advances the previously created query
*	to the next matching entity, if there is one
*/
void ecs_query_next(ecs_t* ecs, ecs_query_t* query);

/**	@brief A functon that gets data for an entity's component
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param query an ecs_query_t pointer, which points to a previously created query object
*	@param component_type an integer, which is the index of the component
*	@returns a void pointer, which holds the data for the component
* 
*	This function gets data for a component on an entity 
*	that is referenced by the previously created query (if any)
*/
void* ecs_query_get_component(ecs_t* ecs, ecs_query_t* query, int component_type);

/**	@brief A function that gets an entity reference from a query
*	@param ecs an ecs_t pointer, which should be a previously created entity component system
*	@param query an ecs_query_t pointer, which points to a previously created query object
*	@returns an ecs_entity_ref_t, which is a reference to an entity object
* 
*	This function gets an entity reference for the current query location.
*	The entity and its sequence are taken from the query and cast as
*	an ecs_entity_ref_t.
*/
ecs_entity_ref_t ecs_query_get_entity(ecs_t* ecs, ecs_query_t* query);