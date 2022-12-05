/** @file transform.h
*	@brief A header file for a 3D transform object (translation, scale, and rotation)
* 
*	These set of operations modify the transform object, performing a variety of 
*	operations on them - multiply, invert, conversion to a matrix
*/
#pragma once

#include "mat4f.h"
#include "quatf.h"
#include "vec3f.h"

/** @brief A transform object
*	
*	This contains three attributes - translation, scale, and rotation.
*	These three attributes are familiar to other engines, such as Unity, 
*	which use the same structure for a transform.
*/
typedef struct transform_t
{
	vec3f_t translation;
	vec3f_t scale;
	quatf_t rotation;
} transform_t;

/** @brief A function to create the identity transform
*	@param transform a transform_t pointer, which holds the transform, scale, and rotation.
*	@returns none
* 
*	Make a transform with no rotation, unit scale, and zero position. This is known as
*	the identity transform. Nothing is returned as transform will be modified.
*/
void transform_identity(transform_t* transform);

/** @brief A function to make a transform into a matrix
*	@param transform a const transform_t pointer, which holds the transform, scale, and rotation.
*	@param output a mat4f_t pointer, which is a typedef for a matrix
*	@returns none
* 
*	Convert a transform to a matrix representation.
*/
void transform_to_matrix(const transform_t* transform, mat4f_t* output);

/**	@brief A function to combine transforms
*	@param result a transform_t pointer, which holds the transform, scale, and rotation.
*	@param t a const transform_t pointer, which holds the transform, scale, and rotation
*	@returns none
* 
*	Combine two transforms -- result and t -- and store the output in result. Each attribute
*	of the transform_t in transform is multiplied with the corresponding attribute in t
*/
void transform_multiply(transform_t* result, const transform_t* t);

/** @brief A function to calculate the inverse of a transform
*	@param transform a transform_t pointer, which holds the transform, scale, and rotation.
*	@returns none
* 
*	Compute a transform's inverse in translation, scale, and rotation. This is done by helper functions
*	that are unique to each attribute (except for translation which can be done with 1/x, 1/y, 1/z
*/
void transform_invert(transform_t* transform);

/** @brief A function to transform a vector by a transform
*	@param transform a const transform_t pointer, which holds the transform, scale, and rotation.
*	@param v a vec3f_t, which is a 3D vector that will be transformed.
*	@returns a vec3f_t, which is the result of transforming the vectory by the transform.
*	
*	Transform a vector by a transform object. Return the resulting vector. This is achieved
*	by scaling the vector, using this scaled vector to modify the rotation, and then adding this
*	rotated vector to the transform's translation.
*/
vec3f_t transform_transform_vec3(const transform_t* transform, vec3f_t v);