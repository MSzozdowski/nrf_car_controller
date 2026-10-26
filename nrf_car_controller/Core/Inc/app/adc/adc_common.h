/*
 * adc_common.h
 *
 *  Created on: Oct 13, 2026
 *      Author: stasz
 */

#ifndef INC_APP_ADC_COMMON_H_
#define INC_APP_ADC_COMMON_H_

#include "adc.h"
#include "tim.h"

#define NO_OF_ADC_CHANNELS 5U
#define NO_OF_SAMPLES_PER_CHANNEL 10U
#define ADC_BUFFER_SIZE (NO_OF_ADC_CHANNELS * NO_OF_SAMPLES_PER_CHANNEL)

#define ADC_REFERENCE_VOLTAGE 3300U
#define ADC_RESOLUTION 4095U

#define ADC_FILTER_RATIO 5
#define ADC_POWER_FILTER_RATIO 10

typedef enum
{
	ADC_LEFT_RIGHT_POT_INDEX = 0U,
	ADC_UP_DOWN_POT_INDEX = 1U,
	ADC_CURR_SENSE_INDEX = 2U,
	ADC_BATT_VOLTAGE_INDEX = 3U,
	ADC_STEERING_TRIM_INDEX = 4U
}adc_id_e;

void adc_common_init(ADC_HandleTypeDef *adc, TIM_HandleTypeDef *tim);
uint16_t adc_common_get_value_from_id(adc_id_e id);

#endif /* INC_APP_ADC_COMMON_H_ */
