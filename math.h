#pragma once

#include <float.h>
#include <math.h>
#include <stdbool.h>

// writing these in .h file, so compiler has to inline these functions
// inline does not make the actual function call, so the compiler puts the body of the function where it was requested
//	This is encouraged because otherwise it might take more time to load function than to actually execute


// Determines if two scalar values are nearly equal
// given the limitations of floating point accuracy.
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

// Linearly interpolate between two floating point values.
// Return the resulting value.
__forceinline float lerp(float begin, float end, float distance)
{
	return (begin * (1.0f - distance)) + (end * distance);
}