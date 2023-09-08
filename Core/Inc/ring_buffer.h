/*
 * ring_buffer.h
 *
 *  Created on: Sep 7, 2023
 *      Author: luke
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#include <stdint.h>

#define fn_ring_buffer_size(ring_buffer) ((ring_buffer)->size)

enum en_ring_buffer_error {
	EN_RING_BUFFER_ERROR__OK = 0,
	EN_RING_BUFFER_ERROR__OVERFLOW = -1,
	EN_RING_BUFFER_ERROR__UNDERFLOW = -2,
};

typedef enum en_ring_buffer_error en_ring_buffer_error_t;

struct st_ring_buffer {
	uint8_t *buffer;
	uint32_t size;
	uint32_t capacity;
	uint32_t write;
	uint32_t read;
};

typedef struct st_ring_buffer st_ring_buffer_t;

void fn_ring_buffer_init(st_ring_buffer_t *restrict ring_buffer,
		uint8_t *restrict buffer, uint32_t capacity);

en_ring_buffer_error_t fn_ring_buffer_write_byte(st_ring_buffer_t *ring_buffer, uint8_t byte);

en_ring_buffer_error_t fn_ring_buffer_write_bytes(st_ring_buffer_t *restrict ring_buffer,
		const uint8_t *restrict bytes, uint32_t n_bytes);

en_ring_buffer_error_t fn_rign_buffer_read_byte(st_ring_buffer_t *restrict ring_buffer,
		uint8_t *restrict byte);

en_ring_buffer_error_t fn_ring_buffer_read_bytes(st_ring_buffer_t *restrict ring_buffer,
		uint8_t *restrict bytes, uint32_t n_bytes);

#endif /* INC_RING_BUFFER_H_ */
