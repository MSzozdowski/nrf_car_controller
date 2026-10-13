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

static volatile uint32_t dma_tick = 0;

static uint16_t adc_vals_dma[ADC_BUFFER_SIZE * 2U] = {0};
static uint16_t adc_output[NO_OF_ADC_CHANNELS] = {0};

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

    dma_tick = HAL_GetTick();
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if(hadc == common_adc)
    {
        calcute_mean_adc_value(&adc_vals_dma[0]);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if(hadc == common_adc)
    {
        calcute_mean_adc_value(&adc_vals_dma[ADC_BUFFER_SIZE]);
        //HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
        APP_PRINT("ConvHalfCpltCallback time = %ld ms\r\n", HAL_GetTick() - dma_tick);
        dma_tick = HAL_GetTick();
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
        adc_output[i] = (uint16_t) ((sum + NO_OF_SAMPLES_PER_CHANNEL / 2) / NO_OF_SAMPLES_PER_CHANNEL);
    }
}
