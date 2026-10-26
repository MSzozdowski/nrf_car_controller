/*
 * button.c
 *
 *  Created on: Oct 24, 2026
 *      Author: stasz
 */

#include "button.h"
#include "gpio.h"

#include <stdbool.h>

static GPIO_PinState button_read_pin(button_t *button);

void button_init(button_t *button, GPIO_TypeDef *button_port, uint16_t button_pin)
{
	button->button_port = button_port;
	button->button_pin = button_pin;
	button->button_state = BUTTON_IDLE;
	button->pressed_flag = false;
}

void button_process(button_t *button)
{
	switch (button->button_state)
	{
		case BUTTON_IDLE:
			if(button_read_pin(button) == GPIO_PIN_RESET)
			{
				button->button_state = BUTTON_DEBOUNCE;
				button->tick = HAL_GetTick();
			}
			break;

		case BUTTON_DEBOUNCE:
			if(HAL_GetTick() - button->tick > BUTTON_DEBOUNCE_TIME)
			{
				button->button_state = BUTTON_PRESSED;
				button->pressed_flag = true;
			}
			else if(button_read_pin(button) == GPIO_PIN_SET)
			{
				button->button_state = BUTTON_IDLE;
			}
			break;

		case BUTTON_PRESSED:
			if(button_read_pin(button) == GPIO_PIN_SET)
			{
				button->button_state = BUTTON_IDLE;
			}
			break;
	}
}

bool button_was_pressed(button_t *button)
{
	if(button->pressed_flag == true)
	{
		button->pressed_flag = false;
		return true;
	}

	return false;
}

static GPIO_PinState button_read_pin(button_t *button)
{
	return HAL_GPIO_ReadPin(button->button_port, button->button_pin);
}
