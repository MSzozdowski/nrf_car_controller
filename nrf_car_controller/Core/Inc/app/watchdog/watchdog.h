/*
 * watchdog.h
 *
 *  Created on: Oct 26, 2026
 *      Author: stasz
 */

#ifndef INC_APP_WATCHDOG_WATCHDOG_H_
#define INC_APP_WATCHDOG_WATCHDOG_H_

#include "iwdg.h"

void watchdog_refresh(IWDG_HandleTypeDef *iwdg);
void watchdog_reset_info(void);

#endif /* INC_APP_WATCHDOG_WATCHDOG_H_ */
