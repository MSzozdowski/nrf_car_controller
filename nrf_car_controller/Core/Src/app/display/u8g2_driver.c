/*
 * u8g2_driver.c
 *
 *  Created on: Oct 21, 2026
 *      Author: stasz
 */

#include "u8g2_driver.h"
#include "u8g2.h"

static uint8_t unused_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

static u8g2_t u8g2;

void u8g2_driver_init(void)
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, unused_callback, unused_callback);
	u8g2_ClearBuffer(&u8g2);
}

u8g2_t* u8g2_driver_get(void)
{
	return &u8g2;
}

static uint8_t unused_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	(void) u8x8;
	(void) msg;
	(void) arg_int;
	(void) arg_ptr;

	return 0;
}
