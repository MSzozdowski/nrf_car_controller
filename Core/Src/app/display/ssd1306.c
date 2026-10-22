/*
 * ssd1306.c
 *
 *  Created on: Oct 19, 2026
 *      Author: stasz
 */
#include "ssd1306.h"
#include "i2c.h"
#include "app_print.h"
#include "u8g2.h"

#include <string.h>
#include <stdbool.h>

static void ss1306_write_data(uint8_t *data, uint16_t size);
static void ssd1306_write_command(uint8_t cmd);

I2C_HandleTypeDef *ssd1306_i2c;
//static uint8_t framebuffer[SSD1306_FRAMEBUFFER_SIZE] = {0};

static volatile bool i2c_busy = false;
static volatile uint32_t display_transmission_tick = 0;
static volatile uint32_t i2c_transmit_time = 0;

void ssd1306_init(I2C_HandleTypeDef *hi2c)
{
    ssd1306_i2c = hi2c;

    ssd1306_write_command(SSD1306_DISPLAYOFF);

    ssd1306_write_command(SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_write_command(0x80);

    ssd1306_write_command(SSD1306_SETMULTIPLEX);
    ssd1306_write_command(SSD1306_LCDHEIGHT - 1);

    ssd1306_write_command(SSD1306_SETDISPLAYOFFSET);
    ssd1306_write_command(0x0);
    ssd1306_write_command(SSD1306_SETSTARTLINE);

    ssd1306_write_command(SSD1306_CHARGEPUMP);
    ssd1306_write_command(0x14);
    ssd1306_write_command(SSD1306_MEMORYMODE);
    ssd1306_write_command(0x00);
    ssd1306_write_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_write_command(SSD1306_COMSCANDEC);

    ssd1306_write_command(SSD1306_SETCOMPINS);
    ssd1306_write_command(0x12);
    ssd1306_write_command(SSD1306_SETCONTRAST);
    ssd1306_write_command(0xFF);

    ssd1306_write_command(SSD1306_SETPRECHARGE);
    ssd1306_write_command(0xF1);

    ssd1306_write_command(SSD1306_SETVCOMDETECT);
    ssd1306_write_command(0x40);
    ssd1306_write_command(SSD1306_DISPLAYALLON_RESUME);
    ssd1306_write_command(SSD1306_NORMALDISPLAY);
    ssd1306_write_command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306_write_command(SSD1306_DISPLAYON);

    //ssd1306_clear();
}

void ssd1306_update(uint8_t *framebuffer)
{
    if(i2c_busy)
    {
        APP_PRINT("I2C is busy can not update \r\n");
        return;
    }

    ssd1306_write_command(SSD1306_PAGEADDR);
    ssd1306_write_command(0);
    ssd1306_write_command((SSD1306_LCDHEIGHT / 8) - 1);
    ssd1306_write_command(SSD1306_COLUMNADDR);
    ssd1306_write_command(0);
    ssd1306_write_command(SSD1306_LCDWIDTH - 1);

    ss1306_write_data(framebuffer, SSD1306_FRAMEBUFFER_SIZE);
}

//void ssd1306_draw_pixel(int16_t x, int16_t y, ssd1306_color_t color)
//{
//    if((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) || (y >= SSD1306_LCDHEIGHT))
//        return;
//
//    switch(color)
//    {
//    case SSD1306_WHITE:
//        framebuffer[x + (y / 8) * SSD1306_LCDWIDTH] |= (1 << (y & 7));
//        break;
//
//    case SSD1306_BLACK:
//        framebuffer[x + (y / 8) * SSD1306_LCDWIDTH] &= ~(1 << (y & 7));
//        break;
//
//    case SSD1306_INVERSE:
//        framebuffer[x + (y / 8) * SSD1306_LCDWIDTH] ^= (1 << (y & 7));
//        break;
//    }
//}

//void ssd1306_clear(void)
//{
//    memset(framebuffer, 0, sizeof(framebuffer));
//}


static void ss1306_write_data(uint8_t *data, uint16_t size)
{
    APP_PRINT("Display update time = %ld ms\r\n", i2c_transmit_time); //1st execution will be 0

    i2c_busy = true;
    display_transmission_tick = HAL_GetTick();
    HAL_I2C_Mem_Write_DMA(ssd1306_i2c, SSD1306_ADDRESS, SSD1306_CONTROL_DATA, 1, data, size);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if(hi2c == ssd1306_i2c)
    {
        i2c_busy = false;
        i2c_transmit_time = HAL_GetTick() - display_transmission_tick;
    }
}

static void ssd1306_write_command(uint8_t cmd)
{
    uint8_t tx_data[] = {SSD1306_CONTROL_COMMAND, cmd};
    HAL_I2C_Master_Transmit(ssd1306_i2c, SSD1306_ADDRESS, tx_data, sizeof(tx_data), SSD1306_CMD_TIMEOUT);
}

void check_display_adress(void)
{
    for(uint16_t addr = 0; addr <= 0x7F; addr++)
    {
        if(HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 2, 10) == HAL_OK)
        {
            APP_PRINT("I2C: device address 0x%X\r\n", addr);
        }
    }
}
