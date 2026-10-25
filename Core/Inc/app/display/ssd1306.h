/*
 * ssd1306.h
 *
 *  Created on: Oct 19, 2026
 *      Author: stasz
 */

#ifndef INC_APP_SSD1306_H_
#define INC_APP_SSD1306_H_

#include "main.h"

#include <stdbool.h>

#define SSD1306_ADDRESS (0x3C << 1)
#define SSD1306_CMD_TIMEOUT 100

#define SSD1306_LCDWIDTH 128
#define SSD1306_LCDHEIGHT 64

#define SSD1306_FRAMEBUFFER_SIZE (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8)

#define SSD1306_CONTROL_DATA    0x40
#define SSD1306_CONTROL_COMMAND 0x00

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_DEACTIVATE_SCROLL 0x2E

typedef enum
{
    SSD1306_BLACK =  0, ///< Draw 'off' pixels
    SSD1306_WHITE = 1,  ///< Draw 'on' pixels
    SSD1306_INVERSE = 2   ///< Invert pixels
} ssd1306_color_t;

void ssd1306_init(I2C_HandleTypeDef * hi2c);
void ssd1306_draw_pixel(int16_t x, int16_t y, ssd1306_color_t color);
void ssd1306_update(uint8_t *framebuffer);
//void ssd1306_clear(void);
bool ssd1306_is_i2c_busy(void);
void check_display_adress(void);

#endif /* INC_APP_SSD1306_H_ */
