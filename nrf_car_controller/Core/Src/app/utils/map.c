/*
 * map.c
 *
 *  Created on: Oct 22, 2026
 *      Author: stasz
 */
#include "main.h"
#include "map.h"

int32_t map(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
	if (in_min >= in_max)
	{
		return out_min;
	}

	if (value <= in_min)
	{
		return out_min;
	}

	if (value >= in_max)
	{
		return out_max;
	}

	int32_t mapped_val = ((value - in_min) * (out_max - out_min)) / ((in_max - in_min)) + out_min;

	return mapped_val;
}
