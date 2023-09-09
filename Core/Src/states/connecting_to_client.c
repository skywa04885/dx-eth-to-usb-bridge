/*
 * connecting_to_client.c
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#include <stm32h723xx.h>
#include <stm32h7xx_hal.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "main.h"
#include "logging.h"
#include "states/connecting_to_client.h"

static void DX_StateMachine_ConnectingToClient_Entry_CloseOldSocket(st_state_machine_t *sm) {
	int ret = -1;

	st_state_machine_client_t *client = &sm->client;

	if (client->fd == -1) {
		return;
	}

	mlog("Open client socket, closing it before proceeding");

	ret = close(client->fd);

	if (ret == -1) {
		mlog("Failed to close client socket, error(%d): %s", errno,
				strerror(errno));

		Error_Handler();
	}

	client->fd = -1;
}

static void DX_StateMachine_ConnectingToClient_Entry_PrepareGreenLED(st_state_machine_t *sm)
{
	sm->connectingToClientState.lastGreenBlinkTick = HAL_GetTick();

	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
}

void fn_connecting_to_client__entry(st_state_machine_t *sm) {
	DX_StateMachine_ConnectingToClient_Entry_CloseOldSocket(sm);
	DX_StateMachine_ConnectingToClient_Entry_PrepareGreenLED(sm);
}

static void DX_StateMachine_ConnectingToClient_Do_BlinkGreenLED(st_state_machine_t *sm)
{
	DX_StateMachine_ConnectingToClientState_t *state = &sm->connectingToClientState;

	uint32_t currentTick;

	currentTick = HAL_GetTick();

	if (currentTick - state->lastGreenBlinkTick < 500U)
	{
		return;
	}

	state->lastGreenBlinkTick = currentTick;

	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}

static void DX_StateMachine_ConnectingToClient_Do_AcceptClient(st_state_machine_t *sm)
{
	st_state_machine_server_t *server = &sm->server;
	st_state_machine_client_t *client = &sm->client;

	// Accepts the client.
	client->fd = accept(server->fd, (struct sockaddr* ) &client->addr,
			&client->socklen);

	// Handles the case that the client connected successfully.
	if (client->fd >= 0) {
		// Prints that we've connected to a socket.
		mlog("Connected to client socket %s:%d",
				inet_ntoa(client->addr.sin_addr.s_addr),
				ntohs(client->addr.sin_port));

		// Triggers the state transition.
		sm->next_state =
				EN_STATE_MACHINE_STATE__READING_COMMAND_HEADER_FROM_SOCKET;

		return;
	}

	// Handles the case where we would block.
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		return;
	}

	// Logs that we failed to accept the client socket.
	mlog("Failed to accept client socket, error (%d): %s", errno,
			strerror(errno));

	// Calls the error handler.
	Error_Handler();
}

void fn_connecting_to_client__loop(st_state_machine_t *sm) {
	DX_StateMachine_ConnectingToClient_Do_BlinkGreenLED(sm);
	DX_StateMachine_ConnectingToClient_Do_AcceptClient(sm);

}

static void DX_StateMachine_ConnectingToClient_Exit_EnableGreenLED(st_state_machine_t *sm)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
}


static void DX_StateMachine_ConnectingToClient_Exit_ConfigureSocket(st_state_machine_t *sm)
{
	int ret = -1;

	st_state_machine_client_t *client = &sm->client;

	// Sets the nonblocking flag.
	ret = fcntl(client->fd, F_SETFL,
			fcntl(client->fd, F_GETFL, 0) | O_NONBLOCK);

	// Handles the case where we failed to make the socket non-blocking.
	if (ret == -1) {
		mlog("Failed to set client socket to non-blocking mode, error (%d): %s",
				errno, strerror(errno));

		Error_Handler();
	}
}


void fn_connecting_to_client__exit(st_state_machine_t *sm) {
	DX_StateMachine_ConnectingToClient_Exit_ConfigureSocket(sm);
	DX_StateMachine_ConnectingToClient_Exit_EnableGreenLED(sm);
}
