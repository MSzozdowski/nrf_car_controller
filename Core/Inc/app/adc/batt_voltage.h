/*
 * batt_voltage.h
 *
 *  Created on: Oct 19, 2026
 *      Author: stasz
 */

#ifndef INC_APP_ADC_BATT_VOLTAGE_H_
#define INC_APP_ADC_BATT_VOLTAGE_H_

#define BAT_R_TOP_OHM 8200U
#define BAT_R_BOTTOM_OHM 5600U

#include "adc_common.h"

typedef struct
{
	adc_id_e id;
	uint16_t r_bat_top_ohm;
	uint16_t r_bat_bot_ohm;
	uint32_t numerator_multiplier;
	uint32_t denominator_multiplier;
}batt_voltage_t;

void batt_voltage_init(batt_voltage_t *batt_voltage, adc_id_e id, uint16_t top_res_val_ohm, uint16_t bot_res_val_ohm);
uint16_t batt_get_voltage(batt_voltage_t *batt_voltage);

#endif /* INC_APP_ADC_BATT_VOLTAGE_H_ */
