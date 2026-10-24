/*
 * joystick.c
 *
 *  Created on: Oct 19, 2026
 *      Author: stasz
 */

#include "main.h"
#include "joystick.h"
#include "adc_common.h"
#include "map.h"

void joystick_init(joystick_t *joystick, adc_id_e id)
{
	if(joystick == NULL)
	{
		return;
	}

	joystick->id = id;
	joystick->adc_value = 0;
}

uint16_t joystick_get_raw_value(joystick_t *joystick)
{
	return adc_common_get_value_from_id(joystick->id);
}

int16_t joystick_get_mapped_value(joystick_t *joystick)
{
	uint16_t adc_val = joystick_get_raw_value(joystick);

	//int16_t mapped = map(adc_val, 0, 4095, -100, 100);
	int16_t mapped = map(adc_val, 0, 4095, JOYSTICK_MAPPED_MIN, JOYSTICK_MAPPED_MAX);

	return mapped;
}
