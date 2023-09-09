/*
 * usb_event.h
 *
 *  Created on: Sep 9, 2023
 *      Author: luker
 */

#ifndef INC_EVENTS_DX_USB_EVENT_H_
#define INC_EVENTS_DX_USB_EVENT_H_

#include "usbh_core.h"
#include <stdint.h>

typedef enum
{
	DX_USB_EVENT__CONNECTED,
	DX_USB_EVENT__DISCONNECTED,
	DX_USB_EVENT__ACTIVE,
} DX_USBEventType_t;

typedef struct
{
	DX_USBEventType_t type;
} DX_USBEvent_t;

void DX_USBEvent_Init(DX_USBEvent_t *usbEvent, DX_USBEventType_t type);

#endif /* INC_EVENTS_DX_USB_EVENT_H_ */
