/*
 * joystick.h
 *
 *  Created on: Oct 19, 2026
 *      Author: stasz
 */

#ifndef INC_APP_ADC_JOYSTICK_H_
#define INC_APP_ADC_JOYSTICK_H_

#include "adc_common.h"

#define JOYSTICK_MAPPED_MIN 0U
#define JOYSTICK_MAPPED_MAX 255U
#define JOYSTICK_MAPPED_CENTER ((JOYSTICK_MAPPED_MIN + JOYSTICK_MAPPED_MAX + 1) / 2)

typedef struct
{
	adc_id_e id;
	uint16_t adc_value;
	int16_t mapped_value;
}joystick_t;

void joystick_init(joystick_t * joystick, adc_id_e id);
uint16_t joystick_get_raw_value(joystick_t * joystick);
int16_t joystick_get_mapped_value(joystick_t * joystick);

#endif /* INC_APP_ADC_JOYSTICK_H_ */
