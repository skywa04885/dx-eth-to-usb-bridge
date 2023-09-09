/*
 * dx_usb_event.c
 *
 *  Created on: Sep 9, 2023
 *      Author: luker
 */

#include "events/dx_usb_event.h"

void DX_USBEvent_Init(DX_USBEvent_t *usbEvent, DX_USBEventType_t type)
{
	usbEvent->type = type;
}
