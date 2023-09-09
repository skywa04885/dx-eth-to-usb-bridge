/*
 * connecting_to_usb.h
 *
 *  Created on: Sep 9, 2023
 *      Author: luker
 */

#ifndef INC_STATES_CONNECTING_TO_USB_H_
#define INC_STATES_CONNECTING_TO_USB_H_

#include "state_machine.h"

void DX_StateMachineState_ConnectingToUSB_Entry(st_state_machine_t *sm);

void DX_StateMachineState_ConnectingToUSB_Do(st_state_machine_t *sm);

void DX_StateMachineState_ConnectingToUSB_Exit(st_state_machine_t *sm);

#endif /* INC_STATES_CONNECTING_TO_USB_H_ */
