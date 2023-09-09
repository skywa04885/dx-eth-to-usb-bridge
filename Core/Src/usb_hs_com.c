/*
 * usb_hs_com.c
 *
 *  Created on: Sep 7, 2023
 *      Author: luke
 */

#include "usb_hs_com.h"
#include "main.h"

void fn_usb_hs_com_init(st_usb_hs_com_state_t *restrict usb_hs_com,
		USBH_HandleTypeDef *restrict husbh) {

	usb_hs_com->husbh = husbh;

	usb_hs_com->connected = false;
}

void fn_usb_hs_com_on_connected_handler(st_usb_hs_com_state_t *usb_hs_com,
		uint8_t device_id) {
	USBH_StatusTypeDef hal_status = USBH_OK;

	// Make sure that we're not connected yet.

	if (usb_hs_com->connected == true) {
		Error_Handler();
	}

	// Set connected to true.

	usb_hs_com->connected = true;

	// Set the device id.

	usb_hs_com->device_id = device_id;

	// Opens the primary pipe.

	hal_status = USBH_OpenPipe(usb_hs_com->husbh,
	USB_HS_COM__PRIMARY_PIPE_NUMBER,
	USB_HS_COM__PRIMARY_ENDPOINT_NUMBER, device_id,
	USB_HS_COM__SPEED, USB_HS_COM__ENDPOINT_TYPE,
	USB_HS_COM__MAX_PACKET_SIZE);

	// Makes sure that we've opened the primary pipe successfully.

	if (hal_status != USBH_OK) {
		fprintf(stderr, "Failed to open primary pipe pipe\r\n");

		Error_Handler();
	}

	// Opens the secondary pipe.

	hal_status = USBH_OpenPipe(usb_hs_com->husbh,
	USB_HS_COM__SECONDARY_PIPE_NUMBER,
	USB_HS_COM__SECONDARY_ENDPOINT_NUMBER, device_id,
	USB_HS_COM__SPEED, USB_HS_COM__ENDPOINT_TYPE,
	USB_HS_COM__MAX_PACKET_SIZE);

	// Makes sure that we've opened the secondary pipe successfully.

	if (hal_status != USBH_OK) {
		fprintf(stderr, "Failed to open secondary pipe pipe\r\n");

		Error_Handler();
	}

}

void fn_usb_hs_com_write_and_read(st_usb_hs_com_state_t *restrict usb_hs_com,
		uint8_t *restrict out_bytes, uint32_t n_out_bytes,
		uint8_t *in_bytes, uint32_t *n_in_bytes)
{
	USBH_StatusTypeDef hal_status = USBH_OK;

	printf("Performing bulk USB transaction, writing %u bytes\r\n", (unsigned int) n_out_bytes);

	// Sends the bulk data.

	hal_status = USBH_BulkSendData(usb_hs_com->husbh,
			out_bytes, n_out_bytes, USB_HS_COM__PRIMARY_PIPE_NUMBER, false);


	// Makes sure that bulk data sending was successful.

	if (hal_status != USBH_OK)
	{
		fprintf(stderr, "Failed to perform bulk USB transaction, could not send data.\r\n");

		Error_Handler();
	}

	// If we shouldn't receive any response just return.

	if (in_bytes == NULL || n_in_bytes == NULL)
	{
		return;
	}

	// Reads the bulk data.

}

void fn_usb_hs_com_on_disconnected_handler(st_usb_hs_com_state_t *usb_hs_com) {
	usb_hs_com->connected = false;
}
