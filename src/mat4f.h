/** @file mat4f.h
*	@brief A header file for the implementation of 4x4 matrices
* 
*	Matrix support. Four columns, four rows.
*	Matrices can be scaled, rotated, multiplied, transformed,
*	inverted, and made orthographic or perspective.
*/
#pragma once

#include <stdbool.h>

/**	@brief A quaternion object
*
*	This holds the x, y, z, and w component of a quaternion
*	along with a 3D vector (represented as a vec3f_t) and a
*	float. This allows for a secondary representation that may
*	provide benefits such as assigning a vec3f_t instead of
*	separately assigning x, y, z.
*/
typedef struct quatf_t quatf_t;

/** @brief A structure containing all information needed for the vector
*
*	x, y, z are stored in the 3D vector
*/
typedef struct vec3f_t vec3f_t;

/**	@brief A base 4x4 matrix object.
* 
*	This struct holds a 2D array of floats which 
*	creates the 4x4 matrix representation.
*/
typedef struct mat4f_t
{
	float data[4][4];
} mat4f_t;

/**	@brief A function to create an identity matrix
*	@param m a mat4f_t pointer, which is a matrix object
*	@returns none
* 
*	Makes the matrix m identity.\n
*	The identity matrix created is as follows:\n
*	1	0	0	0\n
*	0	1	0	0\n
*	0	0	1	0\n
*	0	0	0	1
*/
void mat4f_make_identity(mat4f_t* m);

/**	@brief A function to make a matrix that translates vectors by t
*	@param m a mat4f_t pointer, which is a matrix object
*	@param t a const vec3f_t pointer, which is a vector object
*	@returns none
* 
*	This function first makes m an identity matrix, then sets the 
*	bottom row to each value of t (x, y, z, 1.0f)
*/
void mat4f_make_translation(mat4f_t* m, const vec3f_t* t);

/**	@brief A function to make a matrix that scales vectors by s
*	@param m a mat4f_t pointer, which is a matrix object
*	@param s a const vec3f_t pointer, which is a vector object
*	@returns none
* 
*	This function first makes m an identity matrix, then sets
*	the leftmost (first) column to each value of s (x, y, z, 1.0f)
*/
void mat4f_make_scaling(mat4f_t* m, const vec3f_t* s);

/**	@brief A function to make a matrix that rotates vectors by q
*	@param m a mat4f_t pointer, which is a matrix object
*	@param q a const quatf_t pointer, which is a quaternion object
*	@returns none
* 
*	This function sets each entry in the matrix by converting the
*	quaternion into values that can be translated / equivalently 
*	read in the matrix
*/
void mat4f_make_rotation(mat4f_t* m, const quatf_t* q);

/**	@brief A function to translate a matrix by a translation vector
*	@param m a mat4f_t pointer, which is a matrix object
*	@param t a const vec3f_t pointer, which is a vector object
*	@returns none
* 
*	This function creates a translation using t, then multiplies
*	that translation with m
*/
void mat4f_translate(mat4f_t* m, const vec3f_t* t);

/**	@brief A function to scale a matrix by a scale vector
*	@param m a mat4f_t pointer, which is a matrix object
*	@param s a const vec3f_t pointer, which is a vector object
*	@returns none
* 
*	This function creates a scaling using s, then multiplies
*	that scaling with m
*/
void mat4f_scale(mat4f_t* m, const vec3f_t* s);

/**	@brief A function to rotate a matrix by a quaternion
*	@param m a mat4f_t pointer, which is a matrix object
*	@param q a const quatf_t pointer, which is a quaternion object
*	@returns none
* 
*	This function creates a rotation using q, then multiplies
*	that rotation with m
*/
void mat4f_rotate(mat4f_t* m, const quatf_t* q);

/**	@brief A function to concatenate 2 matrices into a result
*	@param result a mat4f_t pointer, which is the resulting matrix object
*	@param a a mat4f_t pointer, which is the first matrix object
*	@param b a mat4f_t pointer, which is the second matrix object
*	@returns none
* 
*	This function loops through all elements of a and b,
*	multiplying them together and assigning them to result.
*/
void mat4f_mul(mat4f_t* result, const mat4f_t* a, const mat4f_t* b);

/**	@brief A function to concatenate 2 matrices in place
*	@param result a mat4f_t pointer, which is the first matrix object
*	@param m a mat4f_t pointer, which is the second matrix object
*	@returns none
* 
*	This function calls the mul function, saving the result
*	in result.
*/
void mat4f_mul_inplace(mat4f_t* result, const mat4f_t* m);

/**	@brief A function to multiply a vector by a matrix and stores as a vector
*	@param m a mat4f_t pointer, which is the matrix object
*	@param in a const vec3f_t pointer, which is the vector being multiplied
*	@param out a const vec3f_t pointer, which is the vector being modified with the final result
*	@returns none
* 
*	This function multiplies and sums each column (ex. in->x * first col + in->y * second col + ...)
*/
void mat4f_transform(const mat4f_t* m, const vec3f_t* in, vec3f_t* out);

/**	@brief A function to multiply a vector by a matrix in place
*	@param m a mat4f_t pointer, which is the matrix object
*	@param v a const vec3f_t pointer, which is the vector being multiplied (and storing the final result)
*	@returns none
* 
*	This function multiplies and sums each column (ex. in->x * first col + in->y * second col + ...)
*	doing so by using the transform function, but storing the result back in v
*/
void mat4f_transform_inplace(const mat4f_t* m, vec3f_t* v);

/**	@brief A function to attempt to compute a matrix inverse
*	@param m a mat4f_t pointer, which is the matrix object
*	@returns a boolean which represents if the matrix can compute an inverse (true on success; false if determinant = 0)
* 
*	This function attempts to compute a matrix inverse, returning false if the determinant
*	is zero and not changing m; otherwise, true is returned, and the inverse is modified in m
*/
bool mat4f_invert(mat4f_t* m);

/**	@brief A function that computes a perspective projection matrix
*	@param m a mat4f_t pointer, which is the matrix object
*	@param angle a float, which is the field of view angle in radians
*	@param aspect a float, which is the width/height aspect ratio
*	@param z_near a float, which is the near distance
*	@param z_far a float, which is the far distance
*	@returns none
* 
*	This function takes a field of view angle, width/height aspect 
*	ratio, and depth near and far distances to compute
*	a perspective projection matrix.
*/
void mat4f_make_perspective(mat4f_t* m, float angle, float aspect, float z_near, float z_far);

/**	@brief A function that computes an orthographic projection matrix
*	@param m a mat4f_t pointer, which is the matrix object
*	@param left a float which is the left side of the window
*	@param right a float which is the right side of the window
*	@param top a float, which is the top of the window
*	@param bottom a float, which is the bottom of the window
*	@param f a float, which is the far distance
*	@param n a float, which is the near distance
*	@returns none
* 
*	This function takes each edge of the window and a depth
*	near and far distance to compute an orthographic projection matrix.
*/
void mat4f_make_orthographic(mat4f_t* m, float left, float right, float top, float bottom, float f, float n);

/**	@brief A function that creates a view matrix
*	@param m a mat4f_t pointer, which is the matrix object
*	@param eye a const vec3f_t pointer, which is the current eye location
*	@param dir a const vec3f_t pointer, which is the facing direction
*	@param up a const vec3f_t pointer, which is the up vector
*	@returns none
* 
*	This function creates a view matrix given an eye location, facing direction, and up vector.
*/
void mat4f_make_lookat(mat4f_t* m, const vec3f_t* eye, const vec3f_t* dir, const vec3f_t* up);