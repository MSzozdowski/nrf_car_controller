/*
 * current_sense.h
 *
 *  Created on: Oct 22, 2026
 *      Author: stasz
 */

#include "adc_common.h"

#define CURRENT_SENSE_SHUNT_MILLIOHM 330U
#define CURRENT_SENSE_GAIN           50U

typedef struct
{
	adc_id_e id;
	uint16_t shunt_milliohms;
	uint16_t amplifier_gain;
}current_sense_t;

void current_sense_init(current_sense_t *current_sense, adc_id_e id, uint16_t shunt_milliohms, uint16_t amp_gain);
uint16_t current_sense_get_milliamps(current_sense_t *current_sense);
