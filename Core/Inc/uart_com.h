/*
 * uart_com.h
 *
 *  Created on: Sep 7, 2023
 *      Author: luke
 */

#ifndef INC_UART_COM_H_
#define INC_UART_COM_H_

#include <stdbool.h>
#include <stm32h7xx_hal.h>

#include "ring_buffer.h"

#define UART_COM_TX_BYTES_BUFFER_SIZE 64UL

struct st_uart_com_state {
	UART_HandleTypeDef *uart;
	st_ring_buffer_t rx_ring_buffer;
	st_ring_buffer_t tx_ring_buffer;
	bool tx_active;
	bool rx_active;
	uint8_t tx_bytes[UART_COM_TX_BYTES_BUFFER_SIZE];
	uint8_t rx_byte;
};

typedef struct st_uart_com_state st_uart_com_state_t;

void fn_uart_com_init(st_uart_com_state_t *restrict state,
		UART_HandleTypeDef *restrict uart, uint8_t *restrict rx_buffer,
		uint32_t rx_buffer_capacity, uint8_t *restrict tx_buffer,
		uint32_t tx_buffer_capacity);

void fn_uart_com_rx_start(st_uart_com_state_t *state);

void fn_uart_com_rx_isr_handler(st_uart_com_state_t *state);

void fn_uart_com_tx_complete_isr_handler(st_uart_com_state_t *state);

void fn_uart_com_tx_write(st_uart_com_state_t *restrict state,
		const uint8_t *restrict bytes, uint32_t n_bytes);

#endif /* INC_UART_COM_H_ */
