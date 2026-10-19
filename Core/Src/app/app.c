/*
 * app.c
 *
 *  Created on: Oct 13, 2026
 *      Author: stasz
 */
#include "gpio.h"
#include "adc.h"
#include "tim.h"
#include "adc_common.h"

void app_init(void)
{
	adc_common_init(&hadc1, &htim3);
}

void app_loop(void)
{
	HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
	HAL_Delay(1000);
}

