/**	@file quatf.h
*	@brief A header file for the implementation of quaternions.
* 
*	Each quaternion has an x, y, z, and w value along with a
*	3D vector and float
*/
#pragma once

#include "vec3f.h"

/**	@brief A quaternion object
* 
*	This holds the x, y, z, and w component of a quaternion
*	along with a 3D vector (represented as a vec3f_t) and a 
*	float. This allows for a secondary representation that may
*	provide benefits such as assigning a vec3f_t instead of 
*	separately assigning x, y, z.
*/
typedef struct quatf_t
{
	union
	{
		struct { float x, y, z, w; };
		struct { vec3f_t v3; float s; };
	};
} quatf_t;

/**	@brief A function to create the identity quaternion
*	@returns a quatf_t, which is the identity quaternion, having x, y, z all equal zero
*	(meaning no rotation) and w = 1.
*/
__forceinline quatf_t quatf_identity()
{
	return (quatf_t) { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
}

/**	@brief A function that multiples two quaternions
*	@param a a quatf_t, which is the first quaternion
*	@param b a quatf_t, which is the second quaternion
*	@returns a quatf_t, which is the product of the rotations of a and b
* 
*	Combines the rotation of two quaternions -- a and b -- into a new quaternion.
*	This is achieved by taking the cross product of the vec3f of a and b and adding 
*	this to a scaled value of both a and b.
*/
__forceinline quatf_t quatf_mul(quatf_t a, quatf_t b)
{
	quatf_t result;

	result.v3 = vec3f_cross(a.v3, b.v3);
	result.v3 = vec3f_add(result.v3, vec3f_scale(b.v3, a.s));
	result.v3 = vec3f_add(result.v3, vec3f_scale(a.v3, b.s));

	result.s = (a.s * b.s) - vec3f_dot(a.v3, b.v3);

	return result;
}

/**	@brief A function that calculates the conjugate of a quaternion
*	@param q a quatf_t, which is the quaternion we will compute the inverse of
*	@returns a quatf_t, which is the inverse quaternion of q
* 
*	Computes the inverse of a normalized quaternion.
*	This is achieved by negating the vec3f_t component of q.
*/
__forceinline quatf_t quatf_conjugate(quatf_t q)
{
	quatf_t result;
	result.v3 = vec3f_negate(q.v3);
	result.s = q.s;
	return result;
}

/**	@brief A function that rotates a vector by a quaternion
*	@param q a quatf_t, which is the quaternion used for rotation
*	@param v a vec3f_t, which is the vector being rotated
*	@returns a vec3f_t, which is the result of rotating v
* 
*	Rotates a vector by a quaterion.
*	Returns the resulting vector.
*/
__forceinline vec3f_t quatf_rotate_vec(quatf_t q, vec3f_t v)
{
	vec3f_t t = vec3f_scale(vec3f_cross(q.v3, v), 2.0f);
	return vec3f_add(v, vec3f_add(vec3f_scale(t, q.w), vec3f_cross(q.v3, t)));
}

/** @brief A function that converts a quatf_t to an euleur angle
*	@param q a quatf_t, which is the quaternion to be converted
*	@returns a vec3f_t, which is the converted quaternion
* 
*	Converts a quaternion to representation with 3 angles in radians: roll, yaw, pitch.
*/
vec3f_t quatf_to_eulers(quatf_t q);

/**	@brief A function that creates a quatf_t from euleur angles
*	@param euler_angles a vec3f_t, which is the vec3f_t to be converted to a quatf_t
*	@returns a quatf_t, which is the converted vec3f_t
* 
*	Converts roll, yaw, pitch in radians to a quaternion.
*/
quatf_t quatf_from_eulers(vec3f_t euler_angles);