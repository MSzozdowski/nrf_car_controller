/*
 * joystick.h
 *
 *  Created on: Oct 19, 2026
 *      Author: stasz
 */

#ifndef INC_APP_ADC_JOYSTICK_H_
#define INC_APP_ADC_JOYSTICK_H_

#include "adc_common.h"

typedef struct
{
	adc_id_e id;
	uint16_t adc_value;
}joystick_t;

void joystick_init(joystick_t * joystick, adc_id_e id);
uint16_t joystick_get_value(joystick_t * joystick);

#endif /* INC_APP_ADC_JOYSTICK_H_ */
