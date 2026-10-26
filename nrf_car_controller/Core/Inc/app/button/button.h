/*
 * button.h
 *
 *  Created on: Oct 24, 2026
 *      Author: stasz
 */

#ifndef INC_APP_BUTTON_BUTTON_H_
#define INC_APP_BUTTON_BUTTON_H_

#include "gpio.h"
#include <stdbool.h>

#define BUTTON_DEBOUNCE_TIME 20

typedef enum
{
	BUTTON_IDLE,
	BUTTON_DEBOUNCE,
	BUTTON_PRESSED
}button_state_e;

typedef struct
{
	GPIO_TypeDef * button_port;
	uint16_t button_pin;
	button_state_e button_state;
	uint32_t tick;
	bool pressed_flag;
}button_t;

void button_init(button_t *button, GPIO_TypeDef *button_port, uint16_t button_pin);
void button_process(button_t *button);
bool button_was_pressed(button_t *button);

#endif /* INC_APP_BUTTON_BUTTON_H_ */
