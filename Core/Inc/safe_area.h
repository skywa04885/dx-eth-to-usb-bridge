/*
 * save_area.h
 *
 *  Created on: Sep 7, 2023
 *      Author: luke
 */

#ifndef INC_SAFE_AREA_H_
#define INC_SAFE_AREA_H_

#include <stdbool.h>

struct st_safe_area
{
	bool should_enable_irq;
};

typedef struct st_safe_area st_safe_area_t;

void fn_safe_area_enter(st_safe_area_t *safe_area);

void fn_safe_area_leave(st_safe_area_t *safe_area);

#endif /* INC_SAFE_AREA_H_ */
