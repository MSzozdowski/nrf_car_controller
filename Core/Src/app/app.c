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
#include "screen.h"
#include "nrf2401.h"
#include "nrf2401_defs.h"
#include "app_print.h"
#include "button.h"
#include "map.h"

#include <stdio.h>
#include <stdbool.h>

static void app_update_inputs(void);
static void app_update_power_info(void);
static void app_update_display(void);
static void app_update_radio(void);

static void update_light_state(void);
static void update_actual_screen(void);

static button_t lights_button;
static button_t change_page_button;
static joystick_t joystick_acceleration;
static joystick_t joystick_direction;
static batt_voltage_t batt_voltage;
static current_sense_t current_sense;
static nrf2401_t nrf2401;

static bool light_state = false;

static u8g2_t *u8g2;

static uint32_t tick_display = 0;
static uint32_t tick_message_counter = 0;
static uint32_t tick_inputs_update = 0;
static uint32_t tick_radio = 0;
static uint32_t tick_power_info = 0;

static uint8_t messages_with_ack = 0;
static uint8_t accepted_messages = 0;
static uint8_t accepted_messages_stats = 0;

static uint8_t send_invoked = 0;
static uint8_t send_invoked_stats = 0;

static uint8_t fps = 0;
static uint8_t fps_stats = 0;

static uint32_t display_update_ms = 0;

void app_init(void)
{
	adc_common_init(&hadc1, &htim3);
	joystick_init(&joystick_direction, ADC_LEFT_RIGHT_POT_INDEX);
	joystick_init(&joystick_acceleration, ADC_UP_DOWN_POT_INDEX);
	batt_voltage_init(&batt_voltage, ADC_BATT_VOLTAGE_INDEX, BAT_R_TOP_OHM, BAT_R_BOTTOM_OHM);
	current_sense_init(&current_sense, ADC_CURR_SENSE_INDEX, CURRENT_SENSE_SHUNT_MILLIOHM, CURRENT_SENSE_GAIN);
	button_init(&lights_button, LIGHT_STATE_GPIO_Port, LIGHT_STATE_Pin);
	button_init(&change_page_button, BUZZER_STATE_GPIO_Port, BUZZER_STATE_Pin);

	nrf2401_init(&nrf2401, &hspi1, NRF_CE_GPIO_Port, NRF_CE_Pin, NRF_CSN_GPIO_Port, NRF_CSN_Pin);
	nrf2401_config(&nrf2401, NRF2401_MODE_TX);

	ssd1306_init(&hi2c1);
	u8g2_driver_init();
	u8g2 = u8g2_driver_get();
	screen_init(u8g2);

	ssd1306_update(u8g2_GetBufferPtr(u8g2));

	app_update_power_info(); //data should be not empty at 1st frame
}

void app_loop(void)
{
	nrf2401_process(&nrf2401);
	button_process(&lights_button);
	button_process(&change_page_button);

	uint32_t current_tick = HAL_GetTick();

	if(current_tick - tick_inputs_update >= MS_10)
	{
		tick_inputs_update = current_tick;
		app_update_inputs();
	}

	current_tick = HAL_GetTick();
	if(current_tick - tick_radio >= MS_20)
	{
		tick_radio = current_tick;
		app_update_radio();
	}

	current_tick = HAL_GetTick();
	if(current_tick - tick_display >= MS_50 && ssd1306_is_i2c_busy() == false)
	{
		tick_display = current_tick;
		app_update_display();
		fps++;
	}

	current_tick = HAL_GetTick();
	if(current_tick - tick_power_info >= MS_500)
	{
		tick_power_info = current_tick;
		app_update_power_info();
	}

	current_tick = HAL_GetTick();
	if(current_tick - tick_message_counter >= MS_1000)
	{
		tick_message_counter = current_tick;
		messages_with_ack = (uint8_t)nrf2401.succes_tx_counter;
		nrf2401.succes_tx_counter = 0;

		accepted_messages_stats = accepted_messages;
		accepted_messages = 0;

		send_invoked_stats = send_invoked;
		send_invoked = 0;

		fps_stats = fps;
		fps = 0;
	}
}

static void app_update_inputs(void)
{
	joystick_acceleration.mapped_value = joystick_get_mapped_value(&joystick_acceleration);
	joystick_direction.mapped_value = joystick_get_mapped_value(&joystick_direction);
	update_light_state();
}

static void app_update_power_info(void)
{
	batt_voltage.batt_millivolts = batt_get_voltage(&batt_voltage);
	current_sense.milliamps = current_sense_get_milliamps(&current_sense);
}

static void app_update_display(void)
{
	update_actual_screen();
	screen_data_t screen_data;
	screen_data.throttle_mapped = joystick_acceleration.mapped_value;
	screen_data.steering_mapped = joystick_direction.mapped_value;
	screen_data.battery_mv = batt_voltage.batt_millivolts;
	screen_data.current_ma = current_sense.milliamps;
	screen_data.tx_per_second = messages_with_ack;
	screen_data.lights_on = light_state;

	uint32_t start = HAL_GetTick();

	screen_draw(&screen_data);
	ssd1306_update(u8g2_GetBufferPtr(u8g2));

	display_update_ms = HAL_GetTick() - start;
}

static void app_update_radio(void)
{
	uint8_t joy_dir_val = (uint8_t)joystick_direction.mapped_value;
	uint8_t joy_accel_val = (uint8_t)joystick_acceleration.mapped_value;

	uint8_t data[NRF24_PAYLOAD_SIZE] = {joy_accel_val, joy_dir_val, light_state};
	nrf2401_tx_result_e result = nrf2401_send(&nrf2401, data, sizeof(data));
	if(result == NRF2401_TX_ACCEPTED)
	{
		accepted_messages++;
	}
	send_invoked++;
}

static void update_light_state(void)
{
	if (button_was_pressed(&lights_button))
	{
		light_state = !light_state;
	}
}

static void update_actual_screen(void)
{
	if(button_was_pressed(&change_page_button))
	{
		if(screen_get_current_screen() == SCREEN_MAIN)
		{
			screen_change_screen(SCREEN_DEBUG);
		}
		else if(screen_get_current_screen() == SCREEN_DEBUG)
		{
			screen_change_screen(SCREEN_MAIN);
		}
	}
}
