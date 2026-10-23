/*
 * nrf2401.h
 *
 *  Created on: Oct 23, 2026
 *      Author: stasz
 */

#ifndef INC_APP_RADIO_NRF2401_H_
#define INC_APP_RADIO_NRF2401_H_

#define NRF2401_SPI_TIMEOUT_MS 100U

#include "spi.h"
#include "gpio.h"

typedef enum
{
    NRF2401_MODE_TX,
    NRF2401_MODE_RX
}nrf2401_mode_e;

typedef struct
{
	SPI_HandleTypeDef * hspi;

	GPIO_TypeDef * ce_port;
	uint16_t ce_pin;

	GPIO_TypeDef * csn_port;
	uint16_t csn_pin;
}nrf2401_t;

void nrf2401_init(nrf2401_t *nrf2401, SPI_HandleTypeDef * spi, GPIO_TypeDef * ce_port, uint16_t ce_pin, GPIO_TypeDef * csn_port, uint16_t csn_pin);
void nrf2401_config(nrf2401_t *nrf2401, nrf2401_mode_e mode);

#endif /* INC_APP_RADIO_NRF2401_H_ */
