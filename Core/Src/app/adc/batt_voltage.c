/*
 * batt_voltage.c
 *
 *  Created on: Oct 19, 2026
 *      Author: stasz
 */

#include "main.h"
#include "batt_voltage.h"
#include "adc_common.h"

void batt_voltage_init(batt_voltage_t *batt_voltage, adc_id_e id, uint16_t top_res_val_ohm, uint16_t bot_res_val_ohm)
{
	if(batt_voltage == NULL || top_res_val_ohm == 0 || bot_res_val_ohm == 0)
	{
		return;
	}

	batt_voltage->id = id;
	batt_voltage->r_bat_top_ohm = top_res_val_ohm;
	batt_voltage->r_bat_bot_ohm = bot_res_val_ohm;
	batt_voltage->numerator_multiplier = (uint32_t)(top_res_val_ohm + bot_res_val_ohm);
	batt_voltage->denominator_multiplier = bot_res_val_ohm;
}

uint16_t batt_get_voltage(batt_voltage_t *batt_voltage)
{
	if(batt_voltage == NULL)
	{
		return 0;
	}

	uint16_t adc_val = adc_common_get_value_from_id(batt_voltage->id);

	uint64_t numerator = (uint64_t) adc_val * ADC_REFERENCE_VOLTAGE * batt_voltage->numerator_multiplier;
	uint64_t denominator = (uint64_t) ADC_RESOLUTION * batt_voltage->denominator_multiplier;

	return (uint16_t) ((numerator + (denominator / 2U)) / denominator);
}

