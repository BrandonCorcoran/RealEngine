/** @file vec3f.h
*	@brief A header file for 3D vector support
* 
*	This set of operations provides support for 3D vectors.
*	Vectors are able to be created and mathematical operations can be performed on them
*/
#pragma once

#include <stdlib.h>

#include "math.h"

/** @brief A structure containing all information needed for the vector
* 
*	x, y, z are stored in the 3D vector
*/
typedef struct vec3f_t
{
	union
	{
		struct { float x, y, z; };
		float a[3];
	};
} vec3f_t;

/** @brief A function that returns a vec3f that has an x component of 1.0f
*	@returns a vec3f_t that has an x component of 1.0f
*/
__forceinline vec3f_t vec3f_x()
{
	return (vec3f_t) { .x = 1.0f };
}

/** @brief A function that returns a vec3f that has a y component of 1.0f
*	@returns a vec3f_t that has a y component of 1.0f
*/
__forceinline vec3f_t vec3f_y()
{
	return (vec3f_t) { .y = 1.0f };
}

/** @brief A function that returns a vec3f that has a z component of 1.0f
*	@returns a vec3f_t that has a z component of 1.0f
*/
__forceinline vec3f_t vec3f_z()
{
	return (vec3f_t) { .z = 1.0f };
}

/** @brief A function that returns a vec3f that has an x, y, z component of 1.0f
*	@returns a vec3f_t that has an x, y, z component of 1.0f
*/
__forceinline vec3f_t vec3f_one()
{
	return (vec3f_t) { .x = 1.0f, .y = 1.0f, .z = 1.0f };
}

/** @brief A function that returns a vec3f that has an x, y, z component of 0.0f
*	@returns a vec3f_t that has an x, y, z component of 0.0f
*/
__forceinline vec3f_t vec3f_zero()
{
	return (vec3f_t) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
}

/** @brief A function that returns a vec3f that has an x component of -1.0f
*	@returns a vec3f_t that has an x component of -1.0f
* 
*	This defines the forward direction in the engine to be x = -1.0f
*/
__forceinline vec3f_t vec3f_forward()
{
	return (vec3f_t) { .x = -1.0f };
}

/** @brief A function that returns a vec3f that has a y component of 1.0f
*	@returns a vec3f_t that has a y component of 1.0f
*
*	This defines the right direction in the engine to be y = 1.0f
*/
__forceinline vec3f_t vec3f_right()
{
	return (vec3f_t) { .y = 1.0f };
}

/** @brief A function that returns a vec3f that has a z component of 1.0f
*	@returns a vec3f_t that has a z component of 1.0f
*
*	This defines the up direction in the engine to be z = 1.0f
*/
__forceinline vec3f_t vec3f_up()
{
	return (vec3f_t) { .z = 1.0f };
}

/**	@brief A function that negates a vec3f
*	@param v a vec3f_t, which is the initial vector that will be negated
*	@return a vec3f_t, which negates each x, y, z of v (multiplies by -1)
*/
__forceinline vec3f_t vec3f_negate(vec3f_t v)
{
	return (vec3f_t)
	{
		.x = -v.x,
			.y = -v.y,
			.z = -v.z,
	};
}

/**	@brief A function that adds two vec3f
*	@param a a vec3f_t, which is the first vector
*	@param b a vec3f_t, which is the second vector
*	@return a vec3f_t, which adds each a.x, a.y, a.z of a with b.x, b.y, b.z of b
*/
__forceinline vec3f_t vec3f_add(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = a.x + b.x,
			.y = a.y + b.y,
			.z = a.z + b.z
	};
}

/**	@brief A function that subtracts two vec3f
*	@param a a vec3f_t, which is the first vector
*	@param b a vec3f_t, which is the second vector (that subtracts from a)
*	@return a vec3f_t, which subtracts each b.x, b.y, b.z of b from a.x, a.y, a.z of a 
* 
*	This subtraction is done as a - b
*/
__forceinline vec3f_t vec3f_sub(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = a.x - b.x,
			.y = a.y - b.y,
			.z = a.z - b.z
	};
}

/**	@brief A function that multiplies two vec3f
*	@param a a vec3f_t, which is the first vector
*	@param b a vec3f_t, which is the second vector
*	@return a vec3f_t, which multiplies each a.x, a.y, a.z of a with b.x, b.y, b.z of b
*/
__forceinline vec3f_t vec3f_mul(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = a.x * b.x,
			.y = a.y * b.y,
			.z = a.z * b.z
	};
}

/**	@brief A function that finds the minimum of two vec3f (for each component x, y, z)
*	@param a a vec3f_t, which is the first vector
*	@param b a vec3f_t, which is the second vector
*	@return a vec3f_t, which finds the minimum between each vec3f component (ex. minimum of a.x and b.x)
*/
__forceinline vec3f_t vec3f_min(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = __min(a.x, b.x),
			.y = __min(a.y, b.y),
			.z = __min(a.z, b.z),
	};
}

/**	@brief A function that finds the maximum of two vec3f (for each component x, y, z)
*	@param a a vec3f_t, which is the first vector
*	@param b a vec3f_t, which is the second vector
*	@return a vec3f_t, which finds the maximum between each vec3f component (ex. maximum of a.x and b.x)
*/
__forceinline vec3f_t vec3f_max(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = __max(a.x, b.x),
			.y = __max(a.y, b.y),
			.z = __max(a.z, b.z),
	};
}

/**	@brief A function that scales a vec3f by a constant
*	@param v a vec3f_t, which is the vector to be scaled
*	@param f a vec3f_t, which is the factor to scale by
*	@return a vec3f_t, which finds scales each vec3f component (ex. a.x * f)
*/
__forceinline vec3f_t vec3f_scale(vec3f_t v, float f)
{
	return (vec3f_t)
	{
		.x = v.x * f,
			.y = v.y * f,
			.z = v.z * f,
	};
}

/**	@brief A function that lerps between two vectors by a constant
*	@param a a vec3f_t, which is the starting vector
*	@param b a vec3f_t, which is the ending vector
*	@param f a float, which is the constant
*	@return a vec3f_t, which lerps each component of a and b from a to b by f
*/
__forceinline vec3f_t vec3f_lerp(vec3f_t a, vec3f_t b, float f)
{
	return (vec3f_t)
	{
		.x = lerpf(a.x, b.x, f),
			.y = lerpf(a.y, b.y, f),
			.z = lerpf(a.z, b.z, f),
	};
}

/**	@brief A function that finds the magnitude squared of a vec3f
*	@param v a vec3f_t, which is the vector
*	@return a float, which finds magnitude squared of the vec3f (x^2 + y^2 + z^2)
*/
__forceinline float vec3f_mag2(vec3f_t v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

/**	@brief A function that finds the magnitude of a vec3f
*	@param v a vec3f_t, which is the vector
*	@return a float, which finds magnitude of the vec3f sqrtf(x^2 + y^2 + z^2)
*/
__forceinline float vec3f_mag(vec3f_t v)
{
	return sqrtf(vec3f_mag2(v));
}

/**	@brief A function that finds the distance squared between two vectors
*	@param a a vec3f_t, which is the first vector
*	@param b a vec3f_t, which is the second vector
*	@return a float, which finds distance squared of the vec3f
* 
*	This is computed by finding the difference between the vectors then finding their magnitude squared
*/
__forceinline float vec3f_dist2(vec3f_t a, vec3f_t b)
{
	vec3f_t diff = vec3f_sub(a, b);
	return vec3f_mag2(diff);
}

/**	@brief A function that finds the distance between two vectors
*	@param a a vec3f_t, which is the first vector
*	@param b a vec3f_t, which is the second vector
*	@return a float, which finds distance of the vec3f
*
*	This is computed by finding the difference between the vectors then finding their magnitude
*/
__forceinline float vec3f_dist(vec3f_t a, vec3f_t b)
{
	return sqrtf(vec3f_dist2(a, b));
}

/**	@brief A function that normalizes a vector
*	@param v a vec3f_t, which is the vector
*	@return a vec3f_t, which is the normalized vector
*
*	This is computed by finding the magnitude of v and dividing each of its components by said magnitude
*/
__forceinline vec3f_t vec3f_norm(vec3f_t v)
{
	float m = vec3f_mag(v);
	if (almost_equalf(m, 0.0f))
	{
		return v;
	}
	return vec3f_scale(v, 1.0f / m);
}

/**	@brief A function that finds the dot product of two vectors
*	@param a a vec3f_t, which is the first vector
*	@param b a vec3f_t, which is the second vector
*	@return a float, which finds dot product
*/
__forceinline float vec3f_dot(vec3f_t a, vec3f_t b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

/**	@brief A function that finds the reflection vector
*	@param v a vec3f_t, which is the first vector
*	@param n a vec3f_t, which is the second vector
*	@return a vec3f_t, which finds the reflection vector
* 
*	This is computed by finding the dot product of v and n
*	and then scaling a new vector, r, using n and the 
*	result of the dot product. r is then subtracted from v
*/
__forceinline vec3f_t vec3f_reflect(vec3f_t v, vec3f_t n)
{
	float d = vec3f_dot(v, n);
	vec3f_t r = vec3f_scale(n, d * 2.0f);
	return vec3f_sub(v, r);
}

/**	@brief A function that finds the cross product of two vectors
*	@param a a vec3f_t, which is the first vector
*	@param b a vec3f_t, which is the second vector
*	@return a vec3f_t, which finds cross product
*/
__forceinline vec3f_t vec3f_cross(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = a.y * b.z - a.z * b.y,
			.y = a.z * b.x - a.x * b.z,
			.z = a.x * b.y - a.y * b.x,
	};
}