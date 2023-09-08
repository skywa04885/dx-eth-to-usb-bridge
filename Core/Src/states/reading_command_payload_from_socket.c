/*
 * reading_command_payload_from_socket.c
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#include <states/reading_command_header_from_socket.h>
#include "logging.h"
#include "main.h"

void fn_reading_command_payload_from_socket__entry(st_state_machine_t *sm) {
}

void fn_reading_command_payload_from_socket__loop(st_state_machine_t *sm) {
	int ret = -1;

	st_state_machine_client_t *client = &sm->client;

	// Reads the command payload from the client.
	ret = read(client->fd, (void *) sm->command_payload, sm->command_size);

	// Handles the case where we've finished reading the data from the client.
	if (ret > 0) {
		// Goes to the command to USB writing.
		sm->next_state =
				EN_STATE_MACHINE_STATE__WRITING_COMMAND_TO_USB;

		// Don't do anything else.
		return;
	}

	// Handles the case where we received an EOS.
	if (ret == 0) {
		// Logs that we received the EOS.
		mlog("Received an EOS during reading of payload from client socket");

		// Goes back to the connecting state since we lost the connection.
		sm->next_state = EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT;

		// Don't do anything else.
		return;
	}

	// Handles the case where we have to retry due to blocking.
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		return;
	}

	// Logs the error.
	mlog("Failed to read payload from client socket, error (%d): %s", errno,
			strerror(errno));

	// Calls the error handler.
	Error_Handler();
}

void fn_reading_command_payload_from_socket__exit(st_state_machine_t *sm) {

}
