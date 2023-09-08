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
#include "states/connecting_to_client.h"
#include "states/reading_command_header_from_socket.h"
#include "states/reading_command_payload_from_socket.h"

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
	// Prints that we're initializing the state machine.
	mlog("Initializing state machine");

	// Sets the initial state.
	sm->current_state = sm->next_state =
			EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT;

	// Configures the client.
	fn_state_machine_init__prepare_client(sm);

	// Configures the server.
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

	// Binds the server socket.
	rc = bind(server->fd, (struct sockaddr *) &server->addr, sizeof(struct sockaddr_in));

	// Logs the error and calls the error handler the moment we failed
	//  to bind the server socket.
	if (rc == -1)
	{
		mlog("Failed to bind server socket, err (%d): %s", errno,
				strerror(errno));

		Error_Handler();
	}
}

static void fn_state_machine_start__listen_server_socket(st_state_machine_t *sm) {
	st_state_machine_server_t *server = &sm->server;
	int rc = -1;

	// Listens the server socket.
	rc = listen(server->fd, 1);

	// Logs the error and calls the error handler the moment we failed
	//  to listen the server socket.
	if (rc == -1)
	{
		mlog("Failed to listen server socket, err (%d): %s", errno,
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
