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

#define ADC_LEFT_RIGHT_POT_INDEX 0U
#define ADC_UP_DOWN_POT_INDEX 1U
#define ADC_CURR_SENSE_INDEX 2U
#define ADC_BATT_VOLTAGE_INDEX 3U
#define ADC_STEERING_TRIM_INDEX 4U

void adc_common_init(ADC_HandleTypeDef *adc, TIM_HandleTypeDef *tim);

#endif /* INC_APP_ADC_COMMON_H_ */
