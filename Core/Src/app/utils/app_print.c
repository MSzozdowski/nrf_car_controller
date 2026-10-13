/*
 * app_print.c
 *
 *  Created on: Oct 13, 2026
 *      Author: stasz
 */

#include "app_print.h"
#include "usart.h"

int _write(int file, char *ptr, int len)
{
    (void)file;

    if(len <= 0)
    {
        return 0;
    }

    if(HAL_UART_Transmit(&huart2, (uint8_t *)ptr, (uint16_t)len, 1000) != HAL_OK)
    {
        return -1;
    }

    return len;
}
