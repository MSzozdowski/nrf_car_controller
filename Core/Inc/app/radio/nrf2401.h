/*
 * nrf2401.h
 *
 *  Created on: Oct 23, 2026
 *      Author: stasz
 */

#ifndef INC_APP_RADIO_NRF2401_H_
#define INC_APP_RADIO_NRF2401_H_

#define NRF2401_SPI_TIMEOUT_MS 100U
#define NRF2401_TX_TIMEOUT_MS 100U

#include "spi.h"
#include "gpio.h"

typedef enum
{
    NRF2401_STATE_IDLE,
    NRF2401_STATE_CE_HIGH,
    NRF2401_STATE_WAIT_RESULT
} nrf2401_state_e;

typedef enum
{
	NRF2401_INIT,
    NRF2401_TX_OK,
    NRF2401_TX_MAX_RETRIES,
    NRF2401_TX_TIMEOUT,
    NRF2401_TX_INVALID_ARGUMENT,
	NRF2401_TX_BUSY
} nrf2401_tx_result_e;

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

	nrf2401_state_e state;
	nrf2401_tx_result_e tx_result;
	uint32_t tick;

}nrf2401_t;

void nrf2401_init(nrf2401_t *nrf2401, SPI_HandleTypeDef * spi, GPIO_TypeDef * ce_port, uint16_t ce_pin, GPIO_TypeDef * csn_port, uint16_t csn_pin);
void nrf2401_config(nrf2401_t *nrf2401, nrf2401_mode_e mode);
void nrf2401_process(nrf2401_t *nrf2401);
nrf2401_tx_result_e nrf2401_send(nrf2401_t *nrf2401, uint8_t *data, uint8_t length);

#endif /* INC_APP_RADIO_NRF2401_H_ */
