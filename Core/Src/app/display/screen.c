/*
 * screen.c
 *
 *  Created on: Oct 24, 2026
 *      Author: stasz
 */

#include "screen.h"
#include "u8g2.h"
#include "i2c.h"
#include "joystick.h"
#include "ssd1306.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static void screen_draw_main(screen_data_t *data);
static void screen_draw_debug(screen_data_t *data);
static int32_t screen_axis_to_offset(int32_t value);

static u8g2_t *u8g2 = NULL;
static screen_type_e selected_screen = SCREEN_MAIN;
static char message[MSG_SIZE] = {0};

static const uint8_t image_axis_bits[] = {0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x49,0x92,0x24,0x49,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00};

void screen_init(u8g2_t *p_u8g2)
{
	if(p_u8g2 == NULL)
	{
		return;
	}

	u8g2 = p_u8g2;
	selected_screen = SCREEN_MAIN;

    u8g2_ClearBuffer(u8g2);
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);
}

void screen_draw(screen_data_t *data)
{
	if (u8g2 == NULL || data == NULL)
	{
	    return;
	}

    u8g2_ClearBuffer(u8g2);

    switch (selected_screen)
    {
        case SCREEN_MAIN:
            screen_draw_main(data);
            break;

        case SCREEN_DEBUG:
            screen_draw_debug(data);
            break;
    }
}

screen_type_e screen_get_current_screen(void)
{
	return selected_screen;
}

void screen_change_screen(screen_type_e new_screen)
{
	selected_screen = new_screen;
}

static void screen_draw_main(screen_data_t *data)
{
	u8g2_SetFont(u8g2, u8g2_font_6x10_tr);

	// top_line
	u8g2_DrawLine(u8g2, 0, 10, 128, 10);
	// top_divider
	u8g2_DrawLine(u8g2, 63, 10, 63, 0);

	// batt_string
	u8g2_DrawStr(u8g2, 1, 8, "Batt:");
	// batt_voltage
	uint32_t battery_cv = (data->battery_mv + 5) / 10;
	snprintf(message, sizeof(message), "%ld.%02luV", battery_cv / 100U, battery_cv % 100U);
	u8g2_DrawStr(u8g2, 32, 8, message); //6.01V

	// curr_string
	u8g2_DrawStr(u8g2, 67, 8, "Curr:");
	// current_consmpution_val
	snprintf(message, sizeof(message), "%dmA", data->current_ma);
	u8g2_DrawStr(u8g2, 98, 8, message); //"123mA"


	// joystick_area
	u8g2_DrawFrame(u8g2, 2, 21, 41, 41);
	// joysticks_position
	int32_t offset_x = screen_axis_to_offset(data->steering_mapped);
	int32_t offset_y = screen_axis_to_offset(data->throttle_mapped);

	uint16_t x = (uint16_t)(SCREEN_JOYSTICK_X + offset_x);
	uint16_t y = (uint16_t)(SCREEN_JOYSTICK_Y - offset_y);
	u8g2_DrawDisc(u8g2, x, y, SCREEN_DOT_RADIUS, U8G2_DRAW_ALL);

	// axis
	u8g2_DrawXBM(u8g2, 4, 23, 37, 37, image_axis_bits);
	// throttle_string
	u8g2_DrawStr(u8g2, 44, 22, "Throttle:");
	// throttle_val
	snprintf(message, sizeof(message), "%d", data->throttle_mapped);
	u8g2_DrawStr(u8g2, 98, 22, message);
	// steer_string
	u8g2_DrawStr(u8g2, 62, 32, "Steer:");
	// steer_val
	snprintf(message, sizeof(message), "%d", data->steering_mapped);
	u8g2_DrawStr(u8g2, 98, 32, message);

	// lights_string
	u8g2_DrawStr(u8g2, 56, 42, "Lights:");
	// lights_val
	snprintf(message, sizeof(message), "%s", data->lights_on ? "ON" : "OFF");
	u8g2_DrawStr(u8g2, 98, 42, message);
	// message_send_string
	u8g2_DrawStr(u8g2, 58, 61, "TX:");
	// message_send_val
	snprintf(message, sizeof(message), "%dmsg/s", data->tx_per_second);
	u8g2_DrawStr(u8g2, 80, 61, message); //23 msg/s
}

static void screen_draw_debug(screen_data_t *data)
{
	(void)data; // temporary for warning removal
}

static int32_t screen_axis_to_offset(int32_t value)
{
    int32_t minimum = JOYSTICK_MAPPED_MIN; 			//-100
    int32_t center  = JOYSTICK_MAPPED_CENTER;   	//0
    int32_t maximum = JOYSTICK_MAPPED_MAX;      	//100
    int32_t max_offset  = SCREEN_JOYSTICK_MAX_OFFSET;	//17

    if (value <= minimum)
    {
    	return -max_offset;
    }

    if (value >= maximum)
    {
    	return max_offset;
    }

    int32_t distance = abs(center - value); 	//abs(0 - 25) = 25
    int32_t range = (maximum - minimum) / 2;	// (100 - (-100)) / 2 = 100

    int32_t rounding = range / 2;
    int32_t offset_in_px = (distance * max_offset + rounding) / range; // (25 * 17 + 50 / 2) / 50 = 9

    if (value < center)
    {
        return -offset_in_px;
    }

    return offset_in_px;
}
