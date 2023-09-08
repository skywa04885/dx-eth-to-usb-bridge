/*
 * usb_hs_com.h
 *
 *  Created on: Sep 7, 2023
 *      Author: luke
 */

#ifndef INC_USB_HS_COM_H_
#define INC_USB_HS_COM_H_

#include <stdint.h>
#include <stdbool.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal.h>
#include "usbh_core.h"

#define USB_HS_COM__SPEED USBH_SPEED_FULL
#define USB_HS_COM__ENDPOINT_TYPE USBH_EP_BULK
#define USB_HS_COM__MAX_PACKET_SIZE 64
#define USB_HS_COM__PRIMARY_PIPE_NUMBER 1
#define USB_HS_COM__SECONDARY_PIPE_NUMBER 2
#define USB_HS_COM__PRIMARY_ENDPOINT_NUMBER 1
#define USB_HS_COM__SECONDARY_ENDPOINT_NUMBER 2

struct st_usb_hs_com_state {
	USBH_HandleTypeDef *husbh;
	bool connected;
	uint8_t device_id;
};

typedef struct st_usb_hs_com_state st_usb_hs_com_state_t;

void fn_usb_hs_com_init(st_usb_hs_com_state_t *restrict usb_hs_com,
		USBH_HandleTypeDef *restrict husbh);

void fn_usb_hs_com_on_connected_handler(st_usb_hs_com_state_t *usb_hs_com,
		uint8_t device_id);

void fn_usb_hs_com_on_disconnected_handler(st_usb_hs_com_state_t *usb_hs_com);

#endif /* INC_USB_HS_COM_H_ */
