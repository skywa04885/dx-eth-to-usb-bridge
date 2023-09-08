/*
 * fake_heap.c
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#include <stdio.h>

#include "fake_heap.h"

void* fake_heap_malloc(fake_heap_t *fake_heap) {
	uint32_t i;

	// Return NUL if no space is available.

	if (fake_heap->size == fake_heap->capacity) {
		return NULL;
	}

	// Find the element that still can be used.

	for (i = 0; i < fake_heap->capacity; ++i) {
		if (fake_heap->element_usages[i]) {
			continue;
		}

		break;
	}

	fake_heap->element_usages[i] = true;

	// Increase the size of the fake heap.

	++fake_heap->size;

	// Returns the pointer to the element contents.

	return (void*) fake_heap->elements + fake_heap->datatype_size * i;
}

void* fake_heap_calloc(fake_heap_t *fake_heap) {
	void *data = fake_heap_malloc(fake_heap);

	// If we could not allocate return NULL.

	if (data == NULL) {
		return NULL;
	}

	// Clears the contents of the element.

	for (uint32_t i = 0; i < fake_heap->datatype_size; ++i) {
		((uint8_t*) data)[i] = 0x00;
	}

	// Returns the data pointer that has been cleared.

	return data;
}

void fake_heap_free(fake_heap_t *fake_heap, void *p) {
	void *elements_start = fake_heap->elements;
	void *elements_end = fake_heap->elements
			+ fake_heap->capacity * fake_heap->datatype_size;

	// Makes sure that the pointer is in the memory region.

	if (elements_start > p || elements_end < p) {
		fprintf(stderr,
				"Attempt to free pointer %p that's not in the memory region %p to %p\r\n",
				p, elements_start, elements_end);

		Error_Handler();
	}

	uint32_t p_offset = (uint32_t) (p - elements_start);

	// Makes sure that the pointer points to the start of a object.

	if ((p_offset % fake_heap->datatype_size) != 0) {
		fprintf(stderr,
				"Attempt to free pointer %p that's not pointing to the start of a region",
				p);

		Error_Handler();
	}

	// Gets the index of the heap object.

	uint32_t index = p_offset / fake_heap->datatype_size;

	// Sets the use flag to false.

	fake_heap->element_usages[index] = false;

	// Decreases the size of the heap.

	--fake_heap->size;
}
