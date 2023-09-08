/*
 * uart_com.c
 *
 *  Created on: Sep 7, 2023
 *      Author: luke
 */

#include "uart_com.h"
#include "safe_area.h"
#include "main.h"

void fn_uart_com_init(st_uart_com_state_t *restrict state,
		UART_HandleTypeDef *restrict uart, uint8_t *restrict rx_buffer,
		uint32_t rx_buffer_capacity, uint8_t *restrict tx_buffer,
		uint32_t tx_buffer_capacity)
{
	state->uart = uart;
	state->tx_active = false;
	state->rx_active = false;

	fn_ring_buffer_init(&state->rx_ring_buffer, rx_buffer, rx_buffer_capacity);

	fn_ring_buffer_init(&state->tx_ring_buffer, tx_buffer, tx_buffer_capacity);
}

void fn_uart_com_rx_start(st_uart_com_state_t *state)
{
	st_safe_area_t safe_area;

	// Enters the safe area.

	fn_safe_area_enter(&safe_area);

	// Make sure that we're not receiving yet.

	if (state->rx_active == true)
	{
		Error_Handler();
	}

	// Start receiving one byte with a interrupt.

	HAL_UART_Receive_IT(state->uart, &state->rx_byte, sizeof(uint8_t));

	// Sets RX active to true.

	state->rx_active = true;

	// Leaves the safe area.

	fn_safe_area_leave(&safe_area);
}

static void fn_uart_com_write_tx_chunk(st_uart_com_state_t *state)
{
	en_ring_buffer_error_t ring_buffer_error = EN_RING_BUFFER_ERROR__OK;
	HAL_StatusTypeDef hal_status = HAL_OK;
	uint32_t n_bytes;
	st_safe_area_t safe_area;

	// Enters the safe area.

	fn_safe_area_enter(&safe_area);

	// Make sure we're not transmitting yet.
	if (state->tx_active == true)
	{
		Error_Handler();
	}

	// Read the chunk from the TX ring buffer.

	n_bytes = fn_ring_buffer_size(&state->tx_ring_buffer);

	if (n_bytes > UART_COM_TX_BYTES_BUFFER_SIZE)
	{
		n_bytes = UART_COM_TX_BYTES_BUFFER_SIZE;
	}

	ring_buffer_error = fn_ring_buffer_read_bytes(&state->tx_ring_buffer, state->tx_bytes, n_bytes);

	// Ensure the ring buffer reading was successful and no underflow occurred.

	if (ring_buffer_error != EN_RING_BUFFER_ERROR__OK)
	{
		Error_Handler();
	}

	// Start transmitting with the DMA.

	hal_status = HAL_UART_Transmit_DMA(state->uart, state->tx_bytes, n_bytes);

	// Ensure that the DMA transmission has started.

	if (hal_status != HAL_OK)
	{
		Error_Handler();
	}

	// Change the flag to indicate that we're transmitting.

	state->tx_active = true;

	// Leaves the safe area.

	fn_safe_area_leave(&safe_area);
}

void fn_uart_com_rx_isr_handler(st_uart_com_state_t *state)
{
	en_ring_buffer_error_t error = EN_RING_BUFFER_ERROR__OK;
	st_safe_area_t safe_area;

	// Enters the safe area.

	fn_safe_area_enter(&safe_area);

	// Writes the byte to the RX ring buffer.

	error = fn_ring_buffer_write_byte(&state->rx_ring_buffer, state->rx_byte);

	if (error != EN_RING_BUFFER_ERROR__OK)
	{
		Error_Handler();
	}

	// Sets the RX active to false.

	state->rx_active = false;

	// Starts receiving the next byte.

	fn_uart_com_rx_start(state);

	// Leaves the safe area.

	fn_safe_area_leave(&safe_area);
}

void fn_uart_com_tx_complete_isr_handler(st_uart_com_state_t *state)
{
	st_safe_area_t safe_area;

	// Enters the safe area.

	fn_safe_area_enter(&safe_area);

	// Sets transmitting active to false.

	state->tx_active = false;

	// Checks if there is more data to be written.

	if (fn_ring_buffer_size(&state->tx_ring_buffer) > 0)
	{
		fn_uart_com_write_tx_chunk(state);
	}

	// Leaves the safe area.

	fn_safe_area_leave(&safe_area);
}

void fn_uart_com_tx_write(st_uart_com_state_t *restrict state,
		const uint8_t *restrict bytes, uint32_t n_bytes)
{
	en_ring_buffer_error_t ring_buffer_error = EN_RING_BUFFER_ERROR__OK;
	st_safe_area_t safe_area;

	// Enters the safe area.

	fn_safe_area_enter(&safe_area);

	// Writes the bytes to the TX ring buffer.

	ring_buffer_error = fn_ring_buffer_write_bytes(&state->tx_ring_buffer, bytes, n_bytes);

	// Makes sure that there was no overflow.

	if (ring_buffer_error != EN_RING_BUFFER_ERROR__OK)
	{
		Error_Handler();
	}

	// Starts a transmission if we're not transmitting yet.

	if (state->tx_active == false)
	{
		fn_uart_com_write_tx_chunk(state);
	}

	// Leaves the safe area.

	fn_safe_area_leave(&safe_area);
}
