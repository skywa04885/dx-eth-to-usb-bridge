/*
 * safe_area.c
 *
 *  Created on: Sep 7, 2023
 *      Author: luke
 */

#include <stm32h7xx_hal.h>

#include "safe_area.h"

void fn_safe_area_enter(st_safe_area_t *safe_area) {
	if (__get_PRIMASK() & 0x1) {
		safe_area->should_enable_irq = false;
	} else {
		safe_area->should_enable_irq = true;

		__disable_irq();
	}
}

void fn_safe_area_leave(st_safe_area_t *safe_area) {
	if (safe_area->should_enable_irq)
	{
		__enable_irq();
	}
}

