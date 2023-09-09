/*
 * disconnecting_from_client.c
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#include <states/reading_command_header_from_socket.h>
#include "logging.h"
#include "main.h"

void fn_reading_command_header_from_socket__entry(st_state_machine_t *sm) {
	sm->reading_command_header_from_socket_state.n_bytes_read = 0;
}

static void __loop__read(st_state_machine_t *sm) {
	st_reading_command_header_from_socket_state_t *state =
			&sm->reading_command_header_from_socket_state;

	st_state_machine_client_t *client = &sm->client;

	state->read_rc = read(client->fd,
			&((uint8_t* ) &sm->command_size)[state->n_bytes_read],
			sizeof(uint32_t) - state->n_bytes_read);
}

static void __loop__handle_error(st_state_machine_t *sm) {
	switch (errno) {
	case EAGAIN: {
		break;
	}
	case ECONNRESET: {
		sm->next_state = EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT;

		mlog("Connection reset by peer during reading "
				"of payload from client socket");

		break;
	}
	default: {
		mlog("Failed to read header from client socket, error (%d): %s", errno,
				strerror(errno));

		Error_Handler();

		break;
	}
	}
}

static void __loop__handle_eos(st_state_machine_t *sm) {
	mlog("Received an EOS during reading of header from client socket");

	sm->next_state = EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT;
}

static void __loop__handle_data(st_state_machine_t *sm) {
	st_reading_command_header_from_socket_state_t *state =
			&sm->reading_command_header_from_socket_state;

	state->n_bytes_read += (uint32_t) state->read_rc;

	mlog("Received %u bytes of the %u bytes", state->n_bytes_read, sizeof(uint32_t));

	if (state->n_bytes_read < sizeof(uint32_t)) {
		return;
	}

	if (sm->command_size > ST_STATE_MACHINE__COMMAND_PAYLOAD_SIZE) {
		sm->next_state = EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT;

		mlog("Receiving command that would overflow buffer by %u bytes",
				sm->command_size - ST_STATE_MACHINE__COMMAND_PAYLOAD_SIZE);

		return;
	} else {
		mlog("Received command header indicating size of %d bytes",
				sm->command_size);

		sm->next_state =
				EN_STATE_MACHINE_STATE__READING_COMMAND_PAYLOAD_FROM_SOCKET;
	}
}

void fn_reading_command_header_from_socket__loop(st_state_machine_t *sm) {
	st_reading_command_header_from_socket_state_t *state =
			&sm->reading_command_header_from_socket_state;

	__loop__read(sm);

	if (state->read_rc > 0) {
		__loop__handle_data(sm);
	} else if (state->read_rc == 0) {
		__loop__handle_eos(sm);
	} else {
		__loop__handle_error(sm);
	}
}

void fn_reading_command_header_from_socket__exit(st_state_machine_t *sm) {

}
