/*
 * screen.h
 *
 *  Created on: Oct 24, 2026
 *      Author: stasz
 */

#ifndef INC_APP_DISPLAY_SCREEN_H_
#define INC_APP_DISPLAY_SCREEN_H_

#include "main.h"
#include "u8g2.h"
#include <stdbool.h>

#define MSG_SIZE 32U

typedef enum
{
    SCREEN_MAIN,
    SCREEN_DEBUG
}screen_type_e;

typedef struct
{
    int16_t throttle_percent;
    int16_t steering_percent;
    uint16_t battery_mv;
    uint16_t current_ma;
    uint16_t tx_per_second;
    bool lights_on;
}screen_data_t;

void screen_init(u8g2_t *p_u8g2);
void screen_draw(screen_data_t *data);

#endif /* INC_APP_DISPLAY_SCREEN_H_ */
