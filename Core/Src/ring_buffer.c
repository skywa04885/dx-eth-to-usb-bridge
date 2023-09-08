#include <assert.h>

#include "ring_buffer.h"
#include "main.h"

void fn_ring_buffer_init(st_ring_buffer_t *restrict ring_buffer,
		uint8_t *restrict buffer, uint32_t capacity) {
	ring_buffer->capacity = capacity;
	ring_buffer->buffer = buffer;

	ring_buffer->size = 0U;
	ring_buffer->write = 0U;
	ring_buffer->read = 0U;
}

en_ring_buffer_error_t fn_ring_buffer_write_byte(st_ring_buffer_t *ring_buffer,
		uint8_t byte) {
	if (ring_buffer->size == ring_buffer->capacity) {
		return EN_RING_BUFFER_ERROR__OVERFLOW;
	}

	ring_buffer->buffer[ring_buffer->write] = byte;

	ring_buffer->write = (ring_buffer->write + 1) % ring_buffer->capacity;

	++ring_buffer->size;

	return EN_RING_BUFFER_ERROR__OK;
}

en_ring_buffer_error_t fn_ring_buffer_write_bytes(
		st_ring_buffer_t *restrict ring_buffer, const uint8_t *restrict bytes,
		uint32_t n_bytes) {
	en_ring_buffer_error_t error = EN_RING_BUFFER_ERROR__OK;

	if ((ring_buffer->size + n_bytes) > ring_buffer->capacity) {
		return EN_RING_BUFFER_ERROR__OVERFLOW;
	}

	for (uint32_t i = 0; i < n_bytes; ++i) {
		error = fn_ring_buffer_write_byte(ring_buffer, bytes[i]);

		if (error != EN_RING_BUFFER_ERROR__OK) {
			Error_Handler();
		}
	}

	return EN_RING_BUFFER_ERROR__OK;
}

en_ring_buffer_error_t fn_rign_buffer_read_byte(
		st_ring_buffer_t *restrict ring_buffer, uint8_t *restrict byte) {
	if (ring_buffer->size == 0) {
		return EN_RING_BUFFER_ERROR__UNDERFLOW;
	}

	*byte = ring_buffer->buffer[ring_buffer->read];

	ring_buffer->read = (ring_buffer->read + 1) % ring_buffer->capacity;

	--ring_buffer->size;

	return EN_RING_BUFFER_ERROR__OK;
}

en_ring_buffer_error_t fn_ring_buffer_read_bytes(
		st_ring_buffer_t *restrict ring_buffer, uint8_t *restrict bytes,
		uint32_t n_bytes) {
	en_ring_buffer_error_t error = EN_RING_BUFFER_ERROR__OK;

	if (n_bytes > ring_buffer->size) {
		return EN_RING_BUFFER_ERROR__UNDERFLOW;
	}

	for (uint32_t i = 0; i < n_bytes; ++i) {
		error = fn_rign_buffer_read_byte(ring_buffer, &bytes[i]);

		if (error != EN_RING_BUFFER_ERROR__OK) {
			Error_Handler();
		}
	}

	return EN_RING_BUFFER_ERROR__OK;
}
