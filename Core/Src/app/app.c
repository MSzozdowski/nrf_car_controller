/*
 * app.c
 *
 *  Created on: Oct 13, 2026
 *      Author: stasz
 */
#include "gpio.h"
#include "adc.h"
#include "tim.h"
#include "i2c.h"
#include "adc_common.h"
#include "joystick.h"
#include "batt_voltage.h"
#include "ssd1306.h"

joystick_t joystick_acceleration;
joystick_t joystick_direction;
batt_voltage_t batt_voltage;

void app_init(void)
{
	adc_common_init(&hadc1, &htim3);
	joystick_init(&joystick_direction, ADC_LEFT_RIGHT_POT_INDEX);
	joystick_init(&joystick_direction, ADC_UP_DOWN_POT_INDEX);
	batt_voltage_init(&batt_voltage, ADC_BATT_VOLTAGE_INDEX, BAT_R_TOP_OHM, BAT_R_BOTTOM_OHM);
	check_display_adress();

	ssd1306_init(&hi2c1);
	ssd1306_clear();

	uint8_t j = 0;
	for(uint8_t i = 0; i < 10; i++)
	{
		ssd1306_draw_pixel(i, j++, SSD1306_WHITE);
	}
	ssd1306_update();
}

void app_loop(void)
{
	HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
	HAL_Delay(1000);
}

