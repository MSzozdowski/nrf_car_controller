/*
 * watchdog.c
 *
 *  Created on: Oct 26, 2026
 *      Author: stasz
 */

#include "watchdog.h"
#include "iwdg.h"
#include "app_print.h"

void watchdog_refresh(IWDG_HandleTypeDef *iwdg)
{
	if (HAL_IWDG_Refresh(iwdg) != HAL_OK)
	{
		Error_Handler();
	}
}

void watchdog_reset_info(void)
{
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
	{
		APP_PRINT("!!! IWDG RESET !!!!\n\r");
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}
	else
	{
		APP_PRINT("!!! Normal reset !!!!\n\r");
	}
}
