/*
 * fake_heap.h
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#ifndef INC_FAKE_HEAP_H_
#define INC_FAKE_HEAP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define fake_heap_create(NAME, CAPACITY, DATATYPE) \
	uint8_t NAME ## _data[CAPACITY * sizeof(DATATYPE)]; \
	bool NAME ## _element_usages[CAPACITY]; \
	fake_heap_t NAME = { \
		.capacity = CAPACITY, \
		.size = 0U, \
		.datatype_size = sizeof(DATATYPE), \
		.element_usages = NAME ## _element_usages, \
		.elements = NAME ## _data, \
	}

struct fake_heap {
	uint32_t capacity;
	uint32_t size;
	uint32_t datatype_size;
	bool *element_usages;
	void *elements;
};

typedef struct fake_heap fake_heap_t;

/**
 * Allocates memory on the fake heap.
 * @param fake_heap the heap to allocate the memory on.
 */
void* fake_heap_malloc(fake_heap_t *fake_heap);

/**
 * Allocates memory on the fake heap and clears it.
 * @param fake_heap the heap to allocate an element on.
 */
void* fake_heap_calloc(fake_heap_t *fake_heap);

/**
 * Frees the memory allocated with the given pointer in the heap.
 * @param fake_heap the heap to free the memory in.
 * @param p the pointer of the memory to free.
 */
void fake_heap_free(fake_heap_t *fake_heap, void *p);

#endif /* INC_FAKE_HEAP_H_ */
