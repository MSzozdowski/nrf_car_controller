/*
 * adc_common.c
 *
 *  Created on: Oct 13, 2026
 *      Author: stasz
 */

#include "adc_common.h"
#include "adc.h"
#include "tim.h"
#include "app_print.h"

static void calcute_mean_adc_value(uint16_t* adc_vals);

static ADC_HandleTypeDef* common_adc;
static TIM_HandleTypeDef* dma_event_timer;

static uint16_t adc_vals_dma[ADC_BUFFER_SIZE * 2U] = {0};	//*2 to update half while other is being filled with new samples
static uint16_t adc_output[NO_OF_ADC_CHANNELS] = {0}; 		//store mean values of 10 last samples for each channel

void adc_common_init(ADC_HandleTypeDef *adc, TIM_HandleTypeDef *tim)
{
    common_adc = adc;
    dma_event_timer = tim;

    if(HAL_ADCEx_Calibration_Start(common_adc) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_ADC_Start_DMA(common_adc, (uint32_t *)adc_vals_dma, ADC_BUFFER_SIZE * 2U);
    HAL_TIM_Base_Start(tim);
}

uint16_t adc_common_get_value_from_id(adc_id_e id)
{
	return adc_output[id];
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	//update every 10 ms (1kHz timer and 10 samples)
    if(hadc == common_adc)
    {
        calcute_mean_adc_value(&adc_vals_dma[0]);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	//update every 10 ms (1kHz timer and 10 samples)
    if(hadc == common_adc)
    {
        calcute_mean_adc_value(&adc_vals_dma[ADC_BUFFER_SIZE]);
    }
}

static void calcute_mean_adc_value(uint16_t* adc_vals)
{
    for(uint8_t i = 0; i < NO_OF_ADC_CHANNELS; i++)
    {
        uint32_t sum = 0;
        for(uint8_t j = 0; j < NO_OF_SAMPLES_PER_CHANNEL; j++)
        {
            sum += adc_vals[j * NO_OF_ADC_CHANNELS + i];
        }
        uint16_t adc_new_val = (uint16_t) ((sum + NO_OF_SAMPLES_PER_CHANNEL / 2) / NO_OF_SAMPLES_PER_CHANNEL);

        //remove current fluctuations
        if(i == ADC_CURR_SENSE_INDEX)
        {
        	adc_output[i] = adc_output[i] + (((adc_new_val - adc_output[i]) + 5) / 10);
        }
        else
        {
        	adc_output[i] = adc_output[i] + (((adc_new_val - adc_output[i]) + 2) / 5);
        }
    }
}
