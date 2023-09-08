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
}

void fn_reading_command_header_from_socket__loop(st_state_machine_t *sm) {
	int ret = -1;

	st_state_machine_client_t *client = &sm->client;

	// Reads the command size from the client.
	ret = read(client->fd, (void* ) &sm->command_size, sizeof(uint32_t));

	// Handles the case where we've finished reading the data from the client.
	if (ret > 0) {
		// Handles the case where a buffer would overflow.
		if (sm->command_size > ST_STATE_MACHINE__COMMAND_PAYLOAD_SIZE) {
			// Logs that we received the a too large size.
			mlog("Receiving command that would overflow buffer by %d bytes",
					sm->command_size - ST_STATE_MACHINE__COMMAND_PAYLOAD_SIZE);

			// Goes back to the connecting state to connect to a new 'valid' client.
			sm->next_state = EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT;

			// Don't do anything else.
			return;
		}

		// Goes to the payload loading.
		sm->next_state =
				EN_STATE_MACHINE_STATE__READING_COMMAND_PAYLOAD_FROM_SOCKET;

		// Don't do anything else.
		return;
	}

	// Handles the case where we received an EOS.
	if (ret == 0) {
		// Logs that we received the EOS.
		mlog("Received an EOS during reading of header from client socket");

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
	mlog("Failed to read header from client socket, error (%d): %s", errno,
			strerror(errno));

	// Calls the error handler.
	Error_Handler();
}

void fn_reading_command_header_from_socket__exit(st_state_machine_t *sm) {

}
