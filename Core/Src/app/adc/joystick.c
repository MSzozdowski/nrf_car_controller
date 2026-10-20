/*
 * joystick.c
 *
 *  Created on: Oct 19, 2026
 *      Author: stasz
 */

#include "main.h"
#include "joystick.h"
#include "adc_common.h"

void joystick_init(joystick_t * joystick, adc_id_e id)
{
	if(joystick == NULL)
	{
		return;
	}

	joystick->id = id;
	joystick->adc_value = 0;
}

uint16_t joystick_get_value(joystick_t * joystick)
{
	return adc_common_get_value_from_id(joystick->id);
}
