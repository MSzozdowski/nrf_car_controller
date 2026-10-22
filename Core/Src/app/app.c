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
#include "u8g2_driver.h"
#include "u8g2.h"

#include <stdio.h>

#define MSG_SIZE 32U

joystick_t joystick_acceleration;
joystick_t joystick_direction;
batt_voltage_t batt_voltage;
u8g2_t *u8g2;

void app_init(void)
{
	adc_common_init(&hadc1, &htim3);
	joystick_init(&joystick_direction, ADC_LEFT_RIGHT_POT_INDEX);
	joystick_init(&joystick_acceleration, ADC_UP_DOWN_POT_INDEX);
	batt_voltage_init(&batt_voltage, ADC_BATT_VOLTAGE_INDEX, BAT_R_TOP_OHM, BAT_R_BOTTOM_OHM);

	ssd1306_init(&hi2c1);
	u8g2_driver_init();

	u8g2 = u8g2_driver_get();

    u8g2_ClearBuffer(u8g2);
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_6x10_tr);

	ssd1306_update(u8g2_GetBufferPtr(u8g2));
}

void app_loop(void)
{
	u8g2_ClearBuffer(u8g2);

	char message[MSG_SIZE] = {0};

    u8g2_DrawStr(u8g2, 1, 9, "Throttle:");
    u8g2_DrawStr(u8g2, 1, 17, "Steering:");
    u8g2_DrawStr(u8g2, 1, 25, "Current:");
    u8g2_DrawStr(u8g2, 1, 33, "Battery:");

    snprintf(message, sizeof(message), "%d", joystick_get_value(&joystick_acceleration));
    u8g2_DrawStr(u8g2, 60, 9, message);

    snprintf(message, sizeof(message), "%d", joystick_get_value(&joystick_direction));
    u8g2_DrawStr(u8g2, 60, 17, message);

    u8g2_DrawStr(u8g2, 60, 25, "1234");

    snprintf(message, sizeof(message), "%d mV", batt_get_voltage(&batt_voltage));
    u8g2_DrawStr(u8g2, 60, 33, message);

	ssd1306_update(u8g2_GetBufferPtr(u8g2));
	HAL_Delay(100);
}

