/** @file timeofday.h
*	@brief A header file for date and time support
* 
*	A structure defined here holds the seconds since the epoch
*	along with a packed date and packed time. The latter of these
*	two use bitfield members to reduce the memory used.
*/
#pragma once

#include <stdint.h>

/** @brief A structure containing date and time information
* 
*	This structure holds seconds since epoch and a packed date and packed time
*	These members use bitfield members in an effort to reduce memory used.
*	For example, uint32_t day : 5, says that we only need 5 bits to store the day, which
*	makes sense, as 31 < 2^5 = 32; similarly, for month : 4, we only need 4 bits,
*	since 12 < 2^4 = 16
*/
typedef struct time_date_time_t
{
	uint64_t seconds_since_epoch;
	union
	{
		uint32_t packed_date;
		struct
		{
			uint32_t day : 5;
			uint32_t month : 4;
			uint32_t year : 23;
		};
	};

	union
	{
		uint32_t packed_time;
		struct
		{
			uint32_t second : 6;
			uint32_t minute : 6;
			uint32_t hour : 5;
		};
	};
} time_date_time_t;

/** @brief A function to get the current, local date and time
*	@returns a time_date_time_t object, that contains information filled in for 
*	the above attributes in the time_date_time_t structure.
* 
*	This information is found by calling Windows functions, using the file_time 
*	and system_time.
*/
time_date_time_t timeofday_get();