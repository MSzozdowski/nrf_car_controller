/*
 * nrf2401.c
 *
 *  Created on: Oct 23, 2026
 *      Author: stasz
 */

#include "nrf2401.h"
#include "nrf2401_defs.h"
#include "spi.h"
#include "app_print.h"

#include <stdbool.h>

static void nrf2401_write_payload(nrf2401_t *nrf2401, uint8_t *data, uint8_t length);
static void nrf2401_clear_irq_flags(nrf2401_t *nrf2401);
static void nrf2401_flush_tx(nrf2401_t *nrf2401);
static void nrf2401_flush_rx(nrf2401_t *nrf2401);
static void nrf2401_power_up(nrf2401_t *nrf2401);
static void nrf2401_set_mode(nrf2401_t *nrf2401, nrf2401_mode_e mode);
static void nrf2401_set_payload_size(nrf2401_t *nrf2401, uint8_t pipe_number, uint8_t payload_size);
static void nrf2401_set_rx_address(nrf2401_t *nrf2401, uint8_t pipe_number, uint8_t *address);
static void nrf2401_set_tx_address(nrf2401_t *nrf2401, uint8_t *address);
static void nrf2401_set_retransmission(nrf2401_t *nrf2401, uint8_t retransmission_delay, uint8_t retranmission_repeat);
static void nrf2401_configure_pipe(nrf2401_t *nrf2401, uint8_t pipe_number, bool pipe_enabled, bool auto_ack_enabled);
static void nrf2401_set_data_pipe(nrf2401_t *nrf2401, uint8_t pipe_number, bool enabled);
static void nrf2401_enable_auto_ack(nrf2401_t *nrf2401, uint8_t pipe_number, uint8_t enable_ack);
static void nrf2401_set_crc(nrf2401_t *nrf2401, uint8_t crc_length);
static void nrf2401_set_output_power(nrf2401_t *nrf2401, uint8_t power);
static void nrf2401_set_data_rate(nrf2401_t *nrf2401, uint8_t data_rate);
static void nrf2401_set_channel(nrf2401_t *nrf2401, uint8_t channel);
static void nrf2401_set_address_width(nrf2401_t *nrf2401, uint8_t address_width);

static void nrf2401_send_command(nrf2401_t *nrf2401, uint8_t command);
static void nrf2401_write_register_bytes(nrf2401_t *nrf2401, uint8_t reg, uint8_t *data, uint8_t length);
static void nrf2401_read_register_bytes(nrf2401_t *nrf2401, uint8_t reg, uint8_t *data, uint8_t length);
static void nrf2401_write_register(nrf2401_t *nrf2401, uint8_t reg, uint8_t data);
static uint8_t nrf2401_read_register(nrf2401_t *nrf2401, uint8_t reg);
static void nrf2401_spi_write(nrf2401_t *nrf2401, uint8_t *data, uint16_t length);
static void nrf2401_spi_read(nrf2401_t *nrf2401, uint8_t *data, uint16_t length);
static void nrf_ce_low(nrf2401_t *nrf2401);
static void nrf_ce_high(nrf2401_t *nrf2401);
static void nrf_csn_low(nrf2401_t *nrf2401);
static void nrf_csn_high(nrf2401_t *nrf2401);

void nrf2401_init(nrf2401_t *nrf2401, SPI_HandleTypeDef * spi, GPIO_TypeDef * ce_port, uint16_t ce_pin, GPIO_TypeDef * csn_port, uint16_t csn_pin)
{
	if(nrf2401 == NULL || spi == NULL || ce_port == NULL || csn_port == NULL || ce_pin == 0 || csn_pin == 0)
	{
		return;
	}

	nrf2401->hspi = spi;

	nrf2401->ce_port = ce_port;
	nrf2401->ce_pin = ce_pin;

	nrf2401->csn_port = csn_port;
	nrf2401->csn_pin = csn_pin;

	nrf2401->state = NRF2401_STATE_IDLE;
	nrf2401->tx_result = NRF2401_INIT;
	nrf2401->tick = HAL_GetTick();
	nrf2401->succes_tx_counter = 0;

	nrf_ce_low(nrf2401); // low for stanby mode
	nrf_csn_high(nrf2401); // data transmitting by starting csn from high to low

	__HAL_SPI_ENABLE(nrf2401->hspi); // to setup clk and mosi for logic analyzer
}

void nrf2401_config(nrf2401_t *nrf2401, nrf2401_mode_e mode)
{
	nrf_ce_low(nrf2401);

	nrf2401_set_channel(nrf2401, NRF24_CHANNEL);
	nrf2401_set_address_width(nrf2401, NRF24_ADD_WIDTH);
	nrf2401_set_data_rate(nrf2401, NRF24_RF_DR_250KBPS);
	nrf2401_set_output_power(nrf2401, NRF24_PA_PWR_M18dBM);
	nrf2401_set_crc(nrf2401, NRF24_CRC_WIDTH_1B);

	uint8_t address[NRF24_ADD_WIDTH] = {'b', 'd', 'O'};
	switch (mode)
	{
		case NRF2401_MODE_TX:
			nrf2401_configure_pipe(nrf2401, 0U, true, NRF24_EN_AUTO_ACK); //pipe 0 is used for ACK
			nrf2401_set_tx_address(nrf2401, address);
			nrf2401_set_retransmission(nrf2401, NRF24_RETRANSMISSION_DELAY, NRF24_RETRANMISSION_REPEAT);
			break;

		case NRF2401_MODE_RX:
			nrf2401_set_payload_size(nrf2401, NRF24_PIPE_NUMBER, NRF24_PAYLOAD_SIZE);
			nrf2401_configure_pipe(nrf2401, NRF24_PIPE_NUMBER, true, NRF24_EN_AUTO_ACK);
			nrf2401_set_rx_address(nrf2401, NRF24_PIPE_NUMBER, address);
			break;
	}
	nrf2401_flush_rx(nrf2401);
	nrf2401_flush_tx(nrf2401);
	nrf2401_clear_irq_flags(nrf2401);

	nrf2401_set_mode(nrf2401, mode);
	nrf2401_power_up(nrf2401);
	HAL_Delay(5);
}

void nrf2401_process(nrf2401_t *nrf2401)
{
	if (nrf2401 == NULL)
	{
		return;
	}

	switch (nrf2401->state)
	{
	case NRF2401_STATE_IDLE:
		break;

	case NRF2401_STATE_CE_HIGH:
		if (HAL_GetTick() - nrf2401->tick >= 2U)
		{
			nrf_ce_low(nrf2401);
			nrf2401->state = NRF2401_STATE_WAIT_RESULT;
		}
		break;

	case NRF2401_STATE_WAIT_RESULT:;
		uint8_t status = nrf2401_read_register(nrf2401, NRF24_STATUS);

		if (status & (1U << NRF24_MAX_RT))
		{
			nrf2401->tx_result = NRF2401_TX_MAX_RETRIES;
		}
		else if (status & (1U << NRF24_TX_DS))
		{
			nrf2401->tx_result = NRF2401_TX_OK;
			nrf2401->succes_tx_counter++;
		}
		else if (HAL_GetTick() - nrf2401->tick >= NRF2401_TX_TIMEOUT_MS)
		{
			nrf2401->tx_result = NRF2401_TX_TIMEOUT;
		}
		else
		{
			break; //keep waiting for update status register
		}

		if (nrf2401->tx_result != NRF2401_TX_OK)
		{
			nrf2401_flush_tx(nrf2401);
		}

		nrf2401_clear_irq_flags(nrf2401);
		nrf2401->state = NRF2401_STATE_IDLE;
		break;
	}
}

nrf2401_tx_result_e nrf2401_send(nrf2401_t *nrf2401, uint8_t *data, uint8_t length)
{
    if (nrf2401 == NULL || data == NULL || length != NRF24_PAYLOAD_SIZE)
    {
        return NRF2401_TX_INVALID_ARGUMENT;
    }

    if (nrf2401->state != NRF2401_STATE_IDLE)
    {
        return NRF2401_TX_BUSY;
    }

    nrf_ce_low(nrf2401);
    nrf2401_write_payload(nrf2401, data, length);

    nrf_ce_high(nrf2401);
    nrf2401->tick = HAL_GetTick();
    nrf2401->tx_result = NRF2401_TX_BUSY;
    nrf2401->state = NRF2401_STATE_CE_HIGH;

    return NRF2401_TX_ACCEPTED;
}

static void nrf2401_write_payload(nrf2401_t *nrf2401, uint8_t *data, uint8_t length)
{
    if (data == NULL || length == 0U || length > 32U)
    {
        return;
    }

    uint8_t command = NRF24_CMD_W_TX_PAYLOAD;

    nrf_csn_low(nrf2401);

    nrf2401_spi_write(nrf2401, &command, 1);
    nrf2401_spi_write(nrf2401, data, length);

    nrf_csn_high(nrf2401);
}

static void nrf2401_clear_irq_flags(nrf2401_t *nrf2401)
{
    uint8_t flags = (1U << NRF24_RX_DR) | (1U << NRF24_TX_DS) | (1U << NRF24_MAX_RT);
    nrf2401_write_register(nrf2401, NRF24_STATUS, flags);
}

static void nrf2401_flush_tx(nrf2401_t *nrf2401)
{
	nrf2401_send_command(nrf2401, NRF24_CMD_FLUSH_TX);
}


static void nrf2401_flush_rx(nrf2401_t *nrf2401)
{
	nrf2401_send_command(nrf2401, NRF24_CMD_FLUSH_RX);
}

static void nrf2401_power_up(nrf2401_t *nrf2401)
{
    uint8_t config = nrf2401_read_register(nrf2401, NRF24_CONFIG);
    config |= (1U << NRF24_PWR_UP);
    nrf2401_write_register(nrf2401, NRF24_CONFIG, config);
}

static void nrf2401_set_mode(nrf2401_t *nrf2401, nrf2401_mode_e mode)
{
    nrf_ce_low(nrf2401);

    uint8_t config = nrf2401_read_register(nrf2401, NRF24_CONFIG);

    if (mode == NRF2401_MODE_RX)
    {
        config |= (1U << NRF24_PRIM_RX);
    }
    else
    {
        config &= ~(1U << NRF24_PRIM_RX);
    }

    nrf2401_write_register(nrf2401, NRF24_CONFIG, config);
}

static void nrf2401_set_payload_size(nrf2401_t *nrf2401, uint8_t pipe_number, uint8_t payload_size)
{
	if (pipe_number > 5U || payload_size == 0U || payload_size > 32U)
	{
		return;
	}

	uint8_t reg_value = NRF24_RX_PW_P0 + pipe_number;

	nrf2401_write_register(nrf2401, reg_value, payload_size);
}

static void nrf2401_set_rx_address(nrf2401_t *nrf2401, uint8_t pipe_number, uint8_t *address)
{
	if (address == NULL || pipe_number > 5U)
	{
		return;
	}

	uint8_t reg_value = NRF24_RX_ADDR_P0 + pipe_number;

	if (pipe_number == 0 || pipe_number == 1)
	{
		nrf2401_write_register_bytes(nrf2401, reg_value, address, NRF24_ADD_WIDTH);
	}
	else
	{
		nrf2401_write_register(nrf2401, reg_value, address[0]); //pipes 2–5 share the upper address bytes with pipe 1
	}
}

static void nrf2401_set_tx_address(nrf2401_t *nrf2401, uint8_t *address)
{
	if (address == NULL)
	{
		return;
	}
	nrf2401_write_register_bytes(nrf2401, NRF24_TX_ADDR, address, NRF24_ADD_WIDTH);
	nrf2401_write_register_bytes(nrf2401, NRF24_RX_ADDR_P0, address, NRF24_ADD_WIDTH); // pipe 0 receives ACK packets for this TX address.
}

static void nrf2401_set_retransmission(nrf2401_t *nrf2401, uint8_t retransmission_delay, uint8_t retranmission_repeat)
{
	uint8_t reg_value = 0;

	reg_value = (retransmission_delay << 4) ;
	reg_value |= retranmission_repeat;

	nrf2401_write_register(nrf2401, NRF24_SETUP_RETR, reg_value);
}

static void nrf2401_configure_pipe(nrf2401_t *nrf2401, uint8_t pipe_number, bool pipe_enabled, bool auto_ack_enabled)
{
	if(pipe_number > 5)
	{
		return;
	}

	nrf2401_set_data_pipe(nrf2401, pipe_number, pipe_enabled);

	if (pipe_enabled)
	{
	    nrf2401_enable_auto_ack(nrf2401, pipe_number, auto_ack_enabled);
	}
	else
	{
	    nrf2401_enable_auto_ack(nrf2401, pipe_number, false);
	}
}

//helper function, use nrf2401_configure_pipe instead
static void nrf2401_set_data_pipe(nrf2401_t *nrf2401, uint8_t pipe_number, bool enabled)
{
    if (pipe_number > 5U)
        return;

    uint8_t reg_value = nrf2401_read_register(nrf2401, NRF24_EN_RXADDR);

    if (enabled)
    {
        reg_value |= (1U << pipe_number);
    }
    else
    {
        reg_value &= ~(1U << pipe_number);
    }

    nrf2401_write_register(nrf2401, NRF24_EN_RXADDR, reg_value);
}

//helper function, use nrf2401_configure_pipe instead
static void nrf2401_enable_auto_ack(nrf2401_t *nrf2401, uint8_t pipe_number, uint8_t enable_ack)
{
	if(pipe_number > 5)
	{
		return;
	}

	uint8_t reg_value = nrf2401_read_register(nrf2401, NRF24_EN_AA);

	if(enable_ack)
	{
		reg_value |= (1 << pipe_number);
	}
	else
	{
		reg_value &= ~(1 << pipe_number);
	}

	nrf2401_write_register(nrf2401, NRF24_EN_AA, reg_value);
}

static void nrf2401_set_crc(nrf2401_t *nrf2401, uint8_t crc_length)
{
	if (crc_length != NRF24_CRC_WIDTH_1B && crc_length != NRF24_CRC_WIDTH_2B)
	{
		crc_length = NRF24_CRC_WIDTH_1B; //default
	}

	uint8_t reg_value = nrf2401_read_register(nrf2401, NRF24_CONFIG);

	reg_value |= (1U << NRF24_EN_CRC); //enable CRC bit
	reg_value &= ~(1U << NRF24_CRCO); // clear crc
	reg_value |= (crc_length << NRF24_CRCO);; // setup number of crc bytes

	nrf2401_write_register(nrf2401, NRF24_CONFIG, reg_value);
}

static void nrf2401_set_output_power(nrf2401_t *nrf2401, uint8_t power)
{
	if (power > NRF24_PA_PWR_0dBM)
	{
		power = NRF24_PA_PWR_0dBM; //default
	}

	uint8_t reg_value = nrf2401_read_register(nrf2401, NRF24_RF_SETUP);


	reg_value &= ~(3U << NRF24_RF_PWR); // clear RF_PWR bits [2:1].
	reg_value |= (power << NRF24_RF_PWR); // setup power.

	nrf2401_write_register(nrf2401, NRF24_RF_SETUP, reg_value);
}

static void nrf2401_set_data_rate(nrf2401_t *nrf2401, uint8_t data_rate)
{
	if (data_rate != NRF24_RF_DR_250KBPS && data_rate != NRF24_RF_DR_1MBPS && data_rate != NRF24_RF_DR_2MBPS)
	{
		data_rate = NRF24_RF_DR_250KBPS; //default
	}

	uint8_t reg_value = nrf2401_read_register(nrf2401, NRF24_RF_SETUP);

	reg_value &= ~((1U << NRF24_RF_DR_LOW) | (1U << NRF24_RF_DR_HIGH)); // clear data rate bits

	switch (data_rate)
	{
	case NRF24_RF_DR_250KBPS:
		reg_value |= (1U << NRF24_RF_DR_LOW);
		break;

	case NRF24_RF_DR_2MBPS:
		reg_value |= (1U << NRF24_RF_DR_HIGH);
		break;

	case NRF24_RF_DR_1MBPS:
		//00 for 1mbps
		break;
	}

	nrf2401_write_register(nrf2401, NRF24_RF_SETUP, reg_value);
}

static void nrf2401_set_channel(nrf2401_t *nrf2401, uint8_t channel)
{
	if (channel > 125U) //channels for 0 to 125
	{
		channel = 2; //reset value of channel
	}

	nrf2401_write_register(nrf2401, NRF24_RF_CH, channel);
}

static void nrf2401_set_address_width(nrf2401_t *nrf2401, uint8_t address_width)
{
    if (address_width < 3U || address_width > 5U)
    {
    	address_width = 5; //reset value of address width
    }

    uint8_t reg_value = address_width - 2U; // 5 - 2 = 3 (11)

    nrf2401_write_register(nrf2401, NRF24_SETUP_AW, reg_value);
}

static void nrf2401_send_command(nrf2401_t *nrf2401, uint8_t command)
{
	nrf_csn_low(nrf2401);
	nrf2401_spi_write(nrf2401, &command, 1);
	nrf_csn_high(nrf2401);
}

static void nrf2401_write_register_bytes(nrf2401_t *nrf2401, uint8_t reg, uint8_t *data, uint8_t length)
{
	uint8_t command = NRF24_CMD_W_REGISTER | reg;

	nrf_csn_low(nrf2401);

	nrf2401_spi_write(nrf2401, &command, 1);
	nrf2401_spi_write(nrf2401, data, length);

	nrf_csn_high(nrf2401);
}

static void nrf2401_read_register_bytes(nrf2401_t *nrf2401, uint8_t reg, uint8_t *data, uint8_t length)
{
	uint8_t command = NRF24_CMD_R_REGISTER | reg;

	nrf_csn_low(nrf2401);

	nrf2401_spi_write(nrf2401, &command, 1);
	nrf2401_spi_read(nrf2401, data, length);

	nrf_csn_high(nrf2401);
}

static void nrf2401_write_register(nrf2401_t *nrf2401, uint8_t reg, uint8_t data)
{
	uint8_t buffer[2] = {reg | NRF24_CMD_W_REGISTER, data};

	nrf_csn_low(nrf2401);
	nrf2401_spi_write(nrf2401, buffer, sizeof(buffer));
	nrf_csn_high(nrf2401);
}

static uint8_t nrf2401_read_register(nrf2401_t *nrf2401, uint8_t reg)
{
	uint8_t command = NRF24_CMD_R_REGISTER | reg;
	uint8_t value = 0;

	nrf_csn_low(nrf2401);
	nrf2401_spi_write(nrf2401, &command, 1);
	nrf2401_spi_read(nrf2401, &value, 1);
	nrf_csn_high(nrf2401);

	return value;
}

static void nrf2401_spi_write(nrf2401_t *nrf2401, uint8_t *data, uint16_t length)
{
	HAL_SPI_Transmit(nrf2401->hspi, data, length, NRF2401_SPI_TIMEOUT_MS);
}

static void nrf2401_spi_read(nrf2401_t *nrf2401, uint8_t *data, uint16_t length)
{
	HAL_SPI_Receive(nrf2401->hspi, data, length, NRF2401_SPI_TIMEOUT_MS);
}

static void nrf_ce_low(nrf2401_t *nrf2401)
{
	HAL_GPIO_WritePin(nrf2401->ce_port, nrf2401->ce_pin, GPIO_PIN_RESET);
}

static void nrf_ce_high(nrf2401_t *nrf2401)
{
	HAL_GPIO_WritePin(nrf2401->ce_port, nrf2401->ce_pin, GPIO_PIN_SET);
}

static void nrf_csn_low(nrf2401_t *nrf2401)
{
	HAL_GPIO_WritePin(nrf2401->csn_port, nrf2401->csn_pin, GPIO_PIN_RESET);
}

static void nrf_csn_high(nrf2401_t *nrf2401)
{
	HAL_GPIO_WritePin(nrf2401->csn_port, nrf2401->csn_pin, GPIO_PIN_SET);
}
