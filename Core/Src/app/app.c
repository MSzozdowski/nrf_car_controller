/*
 * app.c
 *
 *  Created on: Oct 13, 2026
 *      Author: stasz
 */
#include "gpio.h"

void app_init(void)
{

}

void app_loop(void)
{
	HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
	HAL_Delay(500);
}

