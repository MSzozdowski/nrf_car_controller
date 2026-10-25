/*
 * current_sense.c
 *
 *  Created on: Oct 22, 2026
 *      Author: stasz
 */

#include "current_sense.h"
#include "adc_common.h"

void current_sense_init(current_sense_t *current_sense, adc_id_e id, uint16_t shunt_milliohms, uint16_t amp_gain)
{
	if(current_sense == NULL)
	{
		return;
	}
	current_sense->id = id;
	current_sense->shunt_milliohms = shunt_milliohms;
	current_sense->amplifier_gain = amp_gain;
	current_sense->milliamps = 0;
}

uint16_t current_sense_get_milliamps(current_sense_t *current_sense)
{
	if(current_sense == NULL)
	{
		return 0;
	}

	uint16_t adc_val = adc_common_get_value_from_id(current_sense->id);

	// I[mA] = Vout[mV] × 1000 / (R[mΩ] × gain)
    uint64_t numerator = (uint64_t) adc_val * ADC_REFERENCE_VOLTAGE * 1000U;
    uint64_t denominator = (uint64_t)ADC_RESOLUTION * current_sense->shunt_milliohms * current_sense->amplifier_gain;

    uint16_t milliamps = (uint16_t)((numerator + denominator / 2U) / denominator);
    return milliamps;
}
