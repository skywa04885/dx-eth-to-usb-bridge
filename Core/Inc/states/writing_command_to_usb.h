/*
 * writing_command_to_usb.h
 *
 *  Created on: Sep 8, 2023
 *      Author: luker
 */

#ifndef INC_STATES_WRITING_COMMAND_TO_USB_H_
#define INC_STATES_WRITING_COMMAND_TO_USB_H_

#include "state_machine.h"

void fn_writing_command_to_usb__entry(st_state_machine_t *sm);

void fn_writing_command_to_usb__loop(st_state_machine_t *sm);

void fn_writing_command_to_usb__exit(st_state_machine_t *sm);

#endif /* INC_STATES_WRITING_COMMAND_TO_USB_H_ */
