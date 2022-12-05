/** @file math.h
*	@brief A header file for math support
* 
*	Writing these in .h file, so compiler has to inline these functions\n
*	Inline does not make the actual function call, so the compiler puts the body of the function where it was requested\n
*	This is encouraged because otherwise it might take more time to load function than to actually execute\n
*	This is used in conjunction with vec3f.h to allow for comparisons of if floats are almost equal and to lerp
*	between values
*/
#pragma once

#include <float.h>
#include <math.h>
#include <stdbool.h>

/**	@brief A function to determine if two floats are almost equal
*	@param a a float
*	@param b a float
*	@returns a bool, which will return true if the values are almost equal; false otherwise
* 
*	Determines if two scalar values are nearly equal
*	given the limitations of floating point accuracy.
*/
__forceinline bool almost_equalf(float a, float b)
{
	float diff = fabsf(a - b);			// fabsf gives us 32-bit floating point values; otherwise we would get 64-bit ones
	if (diff <= FLT_EPSILON * 1000.0f)
	{
		return true;					// FLT_EPSILON is a very small number; 1000 is chosen arbitrarily; may not work if really large
	}
	if (diff <= __max(fabsf(a), fabsf(b)) * FLT_EPSILON * 4)
	{
		return true;
	}
	return false;
}

/** @brief A function to lerp between two floats
*	@param begin a float, which is where the lerp begins from
*	@param end a float, which is where the lerp ends
*	@param distance a float, which is over how far the values are lerped
*	@returns a float which is the lerped value
*	
*	Linearly interpolate between two floating point values and return the resulting value
*/
__forceinline float lerp(float begin, float end, float distance)
{
	return (begin * (1.0f - distance)) + (end * distance);
}