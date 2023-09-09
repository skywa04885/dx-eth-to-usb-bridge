#include <stm32h723xx.h>
#include <stm32h7xx_hal.h>

#include "main.h"
#include "events/dx_usb_event.h"
#include "states/connecting_to_usb.h"
#include "logging.h"
#include "settings.h"

extern USBH_HandleTypeDef hUsbHostHS;

void DX_StateMachineState_ConnectingToUSB_Entry_CloseOldSocket(
		st_state_machine_t *sm) {
	// TODO: make this work!
}

void DX_StateMachineState_ConnectingToUSB_Entry_ConfigureYellowLED(
		st_state_machine_t *sm) {
	DX_StateMachine_ConnectingToUSBState_t *state = &sm->connectingToUSBState;

	HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_RESET);

	state->lastYellowLedBlink = HAL_GetTick();
}

void DX_StateMachineState_ConnectingToUSB_Entry(st_state_machine_t *sm) {
	DX_StateMachineState_ConnectingToUSB_Entry_CloseOldSocket(sm);
	DX_StateMachineState_ConnectingToUSB_Entry_ConfigureYellowLED(sm);
}

static void DX_StateMachineState_ConnectingToUSB_Do_CreatePipes(
		st_state_machine_t *sm) {
	USBH_StatusTypeDef usbhStatus = USBH_OK;

	usbhStatus = USBH_OpenPipe(&hUsbHostHS,
	DX_ETH2USB__USB_DEVICE__PRIMARY_PIPE_NO,
	DX_ETH2USB__USB_DEVICE__PRIMARY_ENDPOINT_NO, hUsbHostHS.device.address,
	EP_SPEED_FULL, EP_TYPE_BULK,
	DX_ETH2USB__USB_DEVICE__MAX_PACKET_SIZE);

	if (usbhStatus != USBH_OK) {
		mlog("Failed to open primary pipe");

		Error_Handler();
	}
}

static void DX_StateMachineState_ConnectingToUSB_Do_BlinkYellowLED(
		st_state_machine_t *sm) {
	DX_StateMachine_ConnectingToUSBState_t *state = &sm->connectingToUSBState;

	uint32_t currentTick;

	currentTick = HAL_GetTick();

	if (currentTick - state->lastYellowLedBlink < 500U) {
		return;
	}

	HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);

	state->lastYellowLedBlink = currentTick;
}

static void DX_StateMachineState_ConnectingToUSB_Do_HandleConnect(
		st_state_machine_t *sm) {
	USBH_StatusTypeDef usbhStatus = USBH_OK;
	uint32_t usbEventCount;
	DX_USBEvent_t event;
	osStatus_t osStatus;

	usbEventCount = osMessageQueueGetCount(sm->usbEventQueue);

	if (usbEventCount == 0) {
		return;
	}

	osStatus = osMessageQueueGet(sm->usbEventQueue, &event, 0, 0);

	if (osStatus != osOK) {
		Error_Handler();
	}

	if (event.type != DX_USB_EVENT__CONNECTED) {
		return;
	}

	USBH_DeviceTypeDef *device = &hUsbHostHS.device;
	USBH_DevDescTypeDef *descr = (USBH_DevDescTypeDef*) device->Data;

	while ((usbhStatus = USBH_Get_DevDesc(&hUsbHostHS, 255)) == USBH_BUSY)
	{}

	if (usbhStatus != USBH_OK) {
		mlog("Failed to get device descriptor.");

		Error_Handler();
	}

	if (descr->idVendor != DX_ETH2USB__USB_DEVICE__VENDOR_ID)
	{
		mlog("Connected USB vendor id mismatch, got %04X expected %04X",
				descr->idVendor, DX_ETH2USB__USB_DEVICE__VENDOR_ID);

		return;
	}

	if (descr->idProduct != DX_ETH2USB__USB_DEVICE__PRODUCT_ID)
	{
		mlog("Connected USB product id mismatch, got %04X expected %04X",
				descr->idVendor, DX_ETH2USB__USB_DEVICE__PRODUCT_ID);

		return;
	}

	mlog("USB connected, restoring to previous state \"%s\"",
			fn_state_machine_state_to_string(sm->previousState));

	sm->next_state = sm->previousState;

	DX_StateMachineState_ConnectingToUSB_Do_CreatePipes(sm);
}

void DX_StateMachineState_ConnectingToUSB_Do(st_state_machine_t *sm) {
	DX_StateMachineState_ConnectingToUSB_Do_HandleConnect(sm);
	DX_StateMachineState_ConnectingToUSB_Do_BlinkYellowLED(sm);

}

void DX_StateMachineState_ConnectingToUSB_Exit_TurnOnYellowLED(
		st_state_machine_t *sm) {
	HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_SET);
}

void DX_StateMachineState_ConnectingToUSB_Exit(st_state_machine_t *sm) {
	DX_StateMachineState_ConnectingToUSB_Exit_TurnOnYellowLED(sm);
}
