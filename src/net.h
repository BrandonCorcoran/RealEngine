/** @file net.h
*	@brief A header file for the implementation of networking
* 
*	These operations allow for the creation and destruction of a
*	networking system. This system can have users connect
*	and disconnect from it
*/
#pragma once

#include "ecs.h"

#include <stdbool.h>
#include <stdint.h>

/**	@brief A handle to a network object
* 
*	This will hold all the relevant information that is needed for the
*	networking system. This includes socket information, connections
*	and their corresponding mutexes and any entities and their 
*	types connected in this network.
*/
typedef struct net_t net_t;

/** @brief A handle to a heap object
*
*	This will hold all relevant information that is needed for the heap.
*	More about the heap can be found in heap.h. The heap will be used
*	for memory allocation and deallocation.
*/
typedef struct heap_t heap_t;

/**	@brief A structure holding IP and port information
*	
*	This information will be used with the network object to 
*	know where to connect (i.e. which IP address and on
*	what port)
*/
typedef struct net_address_t
{
	uint8_t ip[4];
	uint16_t port;
} net_address_t;

/**	@brief A type that is used to register entity types
*	
*	This is a part of entity_type_t and also used to register
*	entity types. It contains references to the entity component system,
*	an entity, and the index this entity is found at (also a user).
*/
typedef void(*net_configure_entity_callback_t)(ecs_t* ecs, ecs_entity_ref_t entity, int type, void* user);

/**	@brief A function that creates a network object
*	@param heap a pointer to a heap_t object, which is how memory is managed
*	@param ecs a pointer to an ecs_t pointer, which is an entity component system
*	@returns a net_t pointer, which is the newly created networking object
* 
*	This function creates a new network object. This is necessary to 
*	be able to have address connect to this network object as well
*	as to disconnect those addresses. This object must be destroyed
*	before program termination to ensure any allocated memory is properly deallocated.
*/
net_t* net_create(heap_t* heap, ecs_t* ecs);

/**	@brief A function that destroys a network object
*	@param net a net_t pointer, which is a previously created network object
*	@returns none
* 
*	This function destroys a previously created network object, which
*	must be called to prevent any memory leaks of allocated memory.
*/
void net_destroy(net_t* net);

/**	@brief A function that loops through all connections
*	@param net a net_t pointer, which is a previously created network object
*	@returns none
*
*	This function loops through all connections and
*	sends and receives packages from those connections.
*	It also times out any old connections
*/
void net_update(net_t* net);

/**	@brief A function that attempts to connect an address to a network object
*	@param net a net_t pointer, which is a previously created network object
*	@param address a const net_address_t pointer, which is the address that is looking to connect to the network object
*	@returns none
* 
*	This function attempts to connect an address to a network object.
*	The connection will either be created or a function will 
*	find the previously created connection
*/
void net_connect(net_t* net, const net_address_t* address);

/**	@brief A function that disconnects all addresses connected to a network object
*	@param net a net_t pointer, which is a previously created network object
*	@returns none
* 
*	This function loops through all connetions and destroys them.
*	A mutex is used to ensure this operation is safely completed
*	without any connections being doubly disconnected (or missed)
*/
void net_disconnect_all(net_t* net);

/**	@brief A function that registers an entity type to be used with the ECS
*	@param net a net_t pointer, which is a previously created network object
*	@param type an integer, which is an index to an entity type
*	@param component_mask a uint64_t, which is the component mask of the entity at index type
*	@param replicated_component_mask a uint64_t, which is the replicated component mask of the entity at index type
*	@param configure_callback a net_configure_entity_callback_t, which is stored with the entity at index type
*	@param configure_callback_data a void pointer, which is stored with the entity at index type
*	@returns none
* 
*	This function checks the current number of entity types and if it is 
*	less than the maximum number we can have, then its relevant data is 
*	copied over. The replicated_component_mask is also updated with the 
*	proper size of the entity.
*/
void net_state_register_entity_type(net_t* net, int type, uint64_t component_mask, uint64_t replicated_component_mask, net_configure_entity_callback_t configure_callback, void* configure_callback_data);

/**	@brief A function that registers an instance of an entity to be used with the ECS
*	@param net a net_t pointer, which is a previously created network object
*	@param type an integer, which is an index to an entity type
*	@param entity an ecs_entity_ref_t, which is the reference to the entity itself
*	@returns none
* 
*	This function loops through all entities in the network system
*	and makes sure this entity has already not been registered before
*	registering it (if not already registered)
*/
void net_state_register_entity_instance(net_t* net, int type, ecs_entity_ref_t entity);

/**	@brief A function that attempts to return the provided string as an address
*	@param str a const char*, which is a string representation of an address
*	@param address a net_address_t pointer, which is a structure that may contain address information (if successful)
*	@returns a boolean, which represents if the address info was successfully found (true if successful, false otherwise)
* 
*	This function takes the provided string and attempts to get the address information
*	associated with the address string. If it exists, the IP address is
*	copied over to address. Otherwise false is returned - failure.
*/
bool net_string_to_address(const char* str, net_address_t* address);