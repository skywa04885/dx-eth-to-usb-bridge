/*
 * state_machine.c
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#include <stdlib.h>

#include "logging.h"
#include "main.h"
#include "state_machine.h"
#include "settings.h"

#include "states/connecting_to_client.h"
#include "states/reading_command_header_from_socket.h"
#include "states/reading_command_payload_from_socket.h"
#include "states/connecting_to_usb.h"

#include "events/dx_usb_event.h"

const char* fn_state_machine_state_to_string(en_state_machine_state_t state) {
	switch (state) {
	case EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT:
		return "Connecting to client";
	case EN_STATE_MACHINE_STATE__READING_COMMAND_HEADER_FROM_SOCKET:
		return "Reading command header from socket";
	case EN_STATE_MACHINE_STATE__READING_COMMAND_PAYLOAD_FROM_SOCKET:
		return "Reading command payload from socket";
	case EN_STATE_MACHINE_STATE__WRITING_COMMAND_TO_USB:
		return "Writing command to USB";
	case EN_STATE_MACHINE_STATE__READING_RESPONSE_FROM_USB:
		return "Reading response from USB";
	case EN_STATE_MACHINE_STATE__WRITING_RESPONSE_TO_SOCKET:
		return "Writing response to socket";
	case EN_STATE_MACHINE_STATE__CONNECTING_TO_USB:
		return "Connecting to USB";
	default:
		Error_Handler();
		return NULL;
	}
}

static void fn_state_machine_init__prepare_client(st_state_machine_t *sm) {
	st_state_machine_client_t *client = &sm->client;

	memset(&client->addr, 0, sizeof(struct sockaddr_in));

	client->fd = -1;
}

static void fn_state_machine_init__prepare_server(st_state_machine_t *sm) {
	st_state_machine_server_t *server = &sm->server;

	memset(&server->addr, 0, sizeof(struct sockaddr_in));

	server->fd = -1;

	server->addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	server->addr.sin_port = htons(8000);
	server->addr.sin_family = AF_INET;
}

void fn_state_machine_init(st_state_machine_t *sm) {
	mlog("Initializing state machine");

	sm->usbEventQueue = osMessageQueueNew(
	DX_ETH2USB__STATE_MACHINE__USB_EVENT_MAX_MSG_CNT, sizeof(DX_USBEventType_t),
			NULL);

	if (sm->usbEventQueue == NULL) {
		mlog("Failed to create USB event queue");

		Error_Handler();
	}

	sm->previousState = EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT;
	sm->current_state = EN_STATE_MACHINE_STATE__CONNECTING_TO_USB;
	sm->next_state = EN_STATE_MACHINE_STATE__CONNECTING_TO_USB;

	fn_state_machine_init__prepare_client(sm);

	fn_state_machine_init__prepare_server(sm);
}

static void fn_state_machine_loop__current_handle_events(st_state_machine_t *sm) {
	switch (sm->current_state) {
	case EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT:
		break;
	case EN_STATE_MACHINE_STATE__READING_COMMAND_HEADER_FROM_SOCKET:
		break;
	case EN_STATE_MACHINE_STATE__READING_COMMAND_PAYLOAD_FROM_SOCKET:
		break;
	case EN_STATE_MACHINE_STATE__WRITING_COMMAND_TO_USB:
		break;
	case EN_STATE_MACHINE_STATE__READING_RESPONSE_FROM_USB:
		break;
	case EN_STATE_MACHINE_STATE__WRITING_RESPONSE_TO_SOCKET:
		break;
	default:
		break;
	}
}

static void fn_state_machine_loop__current_entry(st_state_machine_t *sm) {
	mlog("Entering state \"%s\"",
			fn_state_machine_state_to_string(sm->current_state));

	switch (sm->current_state) {
	case EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT:
		fn_connecting_to_client__entry(sm);
		break;
	case EN_STATE_MACHINE_STATE__READING_COMMAND_HEADER_FROM_SOCKET:
		fn_reading_command_header_from_socket__entry(sm);
		break;
	case EN_STATE_MACHINE_STATE__READING_COMMAND_PAYLOAD_FROM_SOCKET:
		fn_reading_command_payload_from_socket__entry(sm);
		break;
	case EN_STATE_MACHINE_STATE__WRITING_COMMAND_TO_USB:
		break;
	case EN_STATE_MACHINE_STATE__READING_RESPONSE_FROM_USB:
		break;
	case EN_STATE_MACHINE_STATE__WRITING_RESPONSE_TO_SOCKET:
		break;
	case EN_STATE_MACHINE_STATE__CONNECTING_TO_USB:
		DX_StateMachineState_ConnectingToUSB_Entry(sm);
		break;
	default:
		break;
	}
}

static void fn_state_machine_loop__current_do(st_state_machine_t *sm) {
	switch (sm->current_state) {
	case EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT:
		fn_connecting_to_client__loop(sm);
		break;
	case EN_STATE_MACHINE_STATE__READING_COMMAND_HEADER_FROM_SOCKET:
		fn_reading_command_header_from_socket__loop(sm);
		break;
	case EN_STATE_MACHINE_STATE__READING_COMMAND_PAYLOAD_FROM_SOCKET:
		fn_reading_command_payload_from_socket__loop(sm);
		break;
	case EN_STATE_MACHINE_STATE__WRITING_COMMAND_TO_USB:
		break;
	case EN_STATE_MACHINE_STATE__READING_RESPONSE_FROM_USB:
		break;
	case EN_STATE_MACHINE_STATE__WRITING_RESPONSE_TO_SOCKET:
		break;
	case EN_STATE_MACHINE_STATE__CONNECTING_TO_USB:
		DX_StateMachineState_ConnectingToUSB_Do(sm);
		break;
	default:
		break;
	}
}

static void fn_state_machine_loop__current_exit(st_state_machine_t *sm) {
	mlog("Exiting state \"%s\"",
			fn_state_machine_state_to_string(sm->current_state));

	switch (sm->current_state) {
	case EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT:
		fn_connecting_to_client__exit(sm);
		break;
	case EN_STATE_MACHINE_STATE__READING_COMMAND_HEADER_FROM_SOCKET:
		fn_reading_command_header_from_socket__exit(sm);
		break;
	case EN_STATE_MACHINE_STATE__READING_COMMAND_PAYLOAD_FROM_SOCKET:
		fn_reading_command_payload_from_socket__exit(sm);
		break;
	case EN_STATE_MACHINE_STATE__WRITING_COMMAND_TO_USB:
		break;
	case EN_STATE_MACHINE_STATE__READING_RESPONSE_FROM_USB:
		break;
	case EN_STATE_MACHINE_STATE__WRITING_RESPONSE_TO_SOCKET:
		break;
	case EN_STATE_MACHINE_STATE__CONNECTING_TO_USB:
		DX_StateMachineState_ConnectingToUSB_Exit(sm);
		break;
	default:
		break;
	}
}

static void fn_state_machine_loop__transition(st_state_machine_t *sm) {
	// Prints that we're performing a state transition.
	mlog("Performing transition from state \"%s\" to state \"%s\"",
			fn_state_machine_state_to_string(sm->current_state),
			fn_state_machine_state_to_string(sm->next_state));

	// Exiting from the current state.
	fn_state_machine_loop__current_exit(sm);

	// Changes the state.
	sm->current_state = sm->next_state;

	// Performs the entry of the new state.
	fn_state_machine_loop__current_entry(sm);
}

static void fn_state_machine_start__create_server_socket(st_state_machine_t *sm) {
	st_state_machine_server_t *server = &sm->server;

	// Creates the socket.
	server->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Logs the error and calls the error handler the moment we failed
	//  to create the server socket.
	if (server->fd == -1) {
		mlog("Failed to create server socket, err (%d): %s", errno,
				strerror(errno));

		Error_Handler();
	}
}

static void fn_state_machine_start__bind_server_socket(st_state_machine_t *sm) {
	st_state_machine_server_t *server = &sm->server;
	int rc = -1;

	rc = bind(server->fd, (struct sockaddr* ) &server->addr,
			sizeof(struct sockaddr_in));

	if (rc == -1) {
		mlog("Failed to bind server socket, err (%d): %s", errno,
				strerror(errno));

		Error_Handler();
	}
}

static void fn_state_machine_start__listen_server_socket(st_state_machine_t *sm) {
	st_state_machine_server_t *server = &sm->server;
	int rc = -1;

	rc = listen(server->fd, 1);

	if (rc == -1) {
		mlog("Failed to listen server socket, err (%d): %s", errno,
				strerror(errno));

		Error_Handler();
	}
}

static void fn_state_machine_start__configure_server_socket(st_state_machine_t *sm) {
	st_state_machine_server_t *server = &sm->server;
	int rc = -1;

	rc = fcntl(server->fd, F_SETFL,
			fcntl(server->fd, F_GETFL, 0) | O_NONBLOCK);

	if (rc == -1) {
		mlog("Failed to make socket non blocking, err (%d): %s", errno,
				strerror(errno));

		Error_Handler();
	}
}

void fn_state_machine_start(st_state_machine_t *sm) {
	// Prints that we're starting the state machine.
	mlog("Starting the state machine.");

	// Creates, binds and listens the server socket.
	fn_state_machine_start__create_server_socket(sm);
	fn_state_machine_start__bind_server_socket(sm);
	fn_state_machine_start__listen_server_socket(sm);
	fn_state_machine_start__configure_server_socket(sm);

	// Calls the entry of the initial state.
	fn_state_machine_loop__current_entry(sm);
}

void fn_state_machine_loop(st_state_machine_t *sm) {
	// Performs a state transition if needed.
	if (sm->current_state != sm->next_state) {
		fn_state_machine_loop__transition(sm);
	}

	// Handle the events for the current state.
	fn_state_machine_loop__current_handle_events(sm);

	// Performs the do of the current state.
	fn_state_machine_loop__current_do(sm);
}
