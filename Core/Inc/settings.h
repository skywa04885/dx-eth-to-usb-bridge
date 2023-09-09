/*
 * settings.h
 *
 *  Created on: Sep 8, 2023
 *      Author: luker
 */

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#include <stdint.h>

// USB Device

#define DX_ETH2USB__USB_DEVICE__PRIMARY_ENDPOINT_NO 2
#define DX_ETH2USB__USB_DEVICE__PRIMARY_PIPE_NO 1
#define DX_ETH2USB__USB_DEVICE__MAX_PACKET_SIZE 64

#define DX_ETH2USB__USB_DEVICE__VENDOR_ID ((uint16_t) 0x16C1)
#define DX_ETH2USB__USB_DEVICE__PRODUCT_ID ((uint16_t) 0x2664)

// State machine

#define DX_ETH2USB__STATE_MACHINE__USB_EVENT_MAX_MSG_CNT 4

#endif /* INC_SETTINGS_H_ */
