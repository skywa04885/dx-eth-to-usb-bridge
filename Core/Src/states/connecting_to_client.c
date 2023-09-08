/*
 * connecting_to_client.c
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#include <stdlib.h>
#include <sys/socket.h>

#include "logging.h"
#include "states/connecting_to_client.h"

void fn_connecting_to_client__entry(st_state_machine_t *sm) {
	int ret = -1;

	st_state_machine_client_t *client = &sm->client;

	// If there was no client in advance, nothing to do.
	if (client->fd == -1) {
		return;
	}

	// Logs that we're closing the previous client.
	mlog("Open client socket, shutting down and closing it before proceeding");

	// Shuts the connection down.
	ret = shutdown(client->fd, SHUT_RDWR);

	// Handles the case where the shutdown failed.
	if (ret == -1) {
		mlog("Failed to shutdown client socket, error(%d): %s", errno,
				strerror(errno));

		Error_Handler();
	}

	// Closes the client file descriptor.
	ret = close(client->fd);

	// Handles the case where the close failed.
	if (ret == -1) {
		mlog("Failed to close client socket, error(%d): %s", errno,
				strerror(errno));

		Error_Handler();
	}

	// Sets the file descriptor to -1 to indicate it was closed.
	client->fd = -1;
}

void fn_connecting_to_client__loop(st_state_machine_t *sm) {
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

void fn_connecting_to_client__exit(st_state_machine_t *sm) {
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
