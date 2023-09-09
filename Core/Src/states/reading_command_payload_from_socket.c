/*
 * reading_command_payload_from_socket.c
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#include <states/reading_command_header_from_socket.h>
#include "logging.h"
#include "main.h"

uint32_t bytes_received;

void fn_reading_command_payload_from_socket__entry(st_state_machine_t *sm) {
	sm->reading_command_payload_from_socket_state.n_bytes_read = 0;
}

static void __loop__read(st_state_machine_t *sm)
{
	st_reading_command_payload_from_socket_state_t *state =
			&sm->reading_command_payload_from_socket_state;
	st_state_machine_client_t *client = &sm->client;

	state->read_rc = read(client->fd, (void* ) &sm->command_payload[state->n_bytes_read],
			sm->command_size - state->n_bytes_read);
}

static void __loop__handle_data(st_state_machine_t *sm)
{
	st_reading_command_payload_from_socket_state_t *state =
			&sm->reading_command_payload_from_socket_state;

	state->n_bytes_read += (uint32_t) state->read_rc;

	mlog("Received %u bytes of the %u bytes", state->n_bytes_read, sm->command_size);

	if (state->n_bytes_read < sm->command_size) {
		return;
	}

	sm->next_state = EN_STATE_MACHINE_STATE__WRITING_COMMAND_TO_USB;
}

static void __loop__handle_eos(st_state_machine_t *sm)
{
	mlog("Received an EOS during reading of payload from client socket");

	sm->next_state = EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT;
}

static void __loop__handle_error(st_state_machine_t *sm)
{
	switch (errno)
	{
	case EAGAIN:
		break;
	case ECONNRESET:
		mlog("Connection reset by peer during reading of payload from client socket");

		sm->next_state = EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT;

		break;
	default:
		mlog("Failed to read payload from client socket, error (%d): %s", errno,
				strerror(errno));

		Error_Handler();

		break;
	}
}

void fn_reading_command_payload_from_socket__loop(st_state_machine_t *sm) {
	st_reading_command_payload_from_socket_state_t *state =
			&sm->reading_command_payload_from_socket_state;

	__loop__read(sm);

	if (state->read_rc > 0) {
		__loop__handle_data(sm);
	} else if (state->read_rc == 0) {
		__loop__handle_eos(sm);
	} else {
		__loop__handle_error(sm);
	}
}

void fn_reading_command_payload_from_socket__exit(st_state_machine_t *sm) {

}
