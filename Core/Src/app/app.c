/*
 * app.c
 *
 *  Created on: Oct 13, 2026
 *      Author: stasz
 */
#include "app.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"
#include "i2c.h"
#include "adc_common.h"
#include "joystick.h"
#include "batt_voltage.h"
#include "current_sense.h"
#include "ssd1306.h"
#include "u8g2_driver.h"
#include "u8g2.h"
#include "nrf2401.h"
#include "nrf2401_defs.h"
#include "app_print.h"
#include "button.h"
#include "map.h"

#include <stdio.h>
#include <stdbool.h>

static uint8_t get_light_state(void);

button_t button;
joystick_t joystick_acceleration;
joystick_t joystick_direction;
batt_voltage_t batt_voltage;
current_sense_t current_sense;
nrf2401_t nrf2401;


u8g2_t *u8g2;

uint32_t tick_display = 0;
uint32_t tick_nrf = 0;

void app_init(void)
{
	adc_common_init(&hadc1, &htim3);
	joystick_init(&joystick_direction, ADC_LEFT_RIGHT_POT_INDEX);
	joystick_init(&joystick_acceleration, ADC_UP_DOWN_POT_INDEX);
	batt_voltage_init(&batt_voltage, ADC_BATT_VOLTAGE_INDEX, BAT_R_TOP_OHM, BAT_R_BOTTOM_OHM);
	current_sense_init(&current_sense, ADC_CURR_SENSE_INDEX, CURRENT_SENSE_SHUNT_MILLIOHM, CURRENT_SENSE_GAIN);
	button_init(&button, LIGHT_STATE_GPIO_Port, LIGHT_STATE_Pin);

	nrf2401_init(&nrf2401, &hspi1, NRF_CE_GPIO_Port, NRF_CE_Pin, NRF_CSN_GPIO_Port, NRF_CSN_Pin);
	nrf2401_config(&nrf2401, NRF2401_MODE_TX);

	ssd1306_init(&hi2c1);
	u8g2_driver_init();

	u8g2 = u8g2_driver_get();

    u8g2_ClearBuffer(u8g2);
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_6x10_tr);

	ssd1306_update(u8g2_GetBufferPtr(u8g2));

	//HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
}

void app_loop(void)
{
	nrf2401_process(&nrf2401);
	button_process(&button);

	if(HAL_GetTick() - tick_display > MS_100)
	{
		tick_display = HAL_GetTick();

		u8g2_ClearBuffer(u8g2);

		char message[MSG_SIZE] = {0};

	    u8g2_DrawStr(u8g2, 1, 9, "Throttle:");
	    u8g2_DrawStr(u8g2, 1, 17, "Steering:");
	    u8g2_DrawStr(u8g2, 1, 25, "Current:");
	    u8g2_DrawStr(u8g2, 1, 33, "Battery:");

	    snprintf(message, sizeof(message), "%d | %d", joystick_get_raw_value(&joystick_acceleration), joystick_get_percent_value(&joystick_acceleration));
	    u8g2_DrawStr(u8g2, 60, 9, message);

	    snprintf(message, sizeof(message), "%d | %d", joystick_get_raw_value(&joystick_direction), joystick_get_percent_value(&joystick_direction));
	    u8g2_DrawStr(u8g2, 60, 17, message);

	    snprintf(message, sizeof(message), "%d mA", current_sense_get_milliamps(&current_sense));
	    u8g2_DrawStr(u8g2, 60, 25, message);

	    snprintf(message, sizeof(message), "%d mV", batt_get_voltage(&batt_voltage));
	    u8g2_DrawStr(u8g2, 60, 33, message);

		ssd1306_update(u8g2_GetBufferPtr(u8g2));

		//APP_PRINT("Hello %d \r\n", HAL_GetTick() - tick);
	}

	if(HAL_GetTick() - tick_nrf > MS_20)
	{
		tick_nrf = HAL_GetTick();

		uint8_t joy_dir_val = (uint8_t)map(joystick_get_raw_value(&joystick_direction), 0, 4095, 0, 255);
		uint8_t joy_accel_val = (uint8_t)map(joystick_get_raw_value(&joystick_acceleration), 0, 4095, 0, 255);

		uint8_t light_state = (uint8_t)get_light_state();

		uint8_t data[NRF24_PAYLOAD_SIZE] = {joy_accel_val, joy_dir_val, light_state};
		nrf2401_send(&nrf2401, data, sizeof(data));

//		if(result != NRF2401_TX_OK)
//		{
//			APP_PRINT("nrf message send failed with status %d \r\n", result);
//		}
	}
}

static uint8_t get_light_state(void)
{
	static bool light_state = false;

	if(button_was_pressed(&button))
	{
	    light_state = !light_state;
	}

	return light_state;
}
