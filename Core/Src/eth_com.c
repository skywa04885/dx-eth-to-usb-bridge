/*
 * eth_com.c
 *
 *  Created on: Sep 7, 2023
 *      Author: luke
 */

#include <stdio.h>
#include <stm32h7xx_hal.h>
#include <stm32h723xx.h>

#include "main.h"
#include "safe_area.h"
#include "eth_com.h"

static void fn_eth_com__server_task(void *_server);
static void fn_eth_com__read_task(void *_server);
static void fn_eth_com__write_task(void *_server);

void fn_eth_com_server_init(st_eth_com_server_state_t *restrict server,
		uint8_t *restrict tx_buffer, uint32_t tx_buffer_capacity,
		uint8_t *restrict rx_buffer, uint32_t rx_buffer_capacity) {
	fn_ring_buffer_init(&server->tx_ring_buffer, tx_buffer, tx_buffer_capacity);

	// Configures the client.
	memset(&server->client_addr, 0, sizeof(struct sockaddr_in));

	server->client_fd = -1;
	server->client_connected = false;

	// Configures the server.
	memset(&server->server_addr, 0, sizeof(struct sockaddr_in));
	server->server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	server->server_addr.sin_port = htons(8000);
	server->server_addr.sin_family = AF_INET;

	server->server_fd = -1;
	server->server_created = false;
	server->server_listening = false;
	server->server_bound = false;

	// Starts the server, reading and writing task.
	server->server_task = osThreadNew(fn_eth_com__server_task, server, NULL);
	server->reading_task = osThreadNew(fn_eth_com__read_task, server, NULL);
	server->writing_task = osThreadNew(fn_eth_com__write_task, server, NULL);
}

/**
 * Disconnects the client from the server.
 * @param server the server state.
 */
static void fn_eth_com__disconnect_client(st_eth_com_server_state_t *server) {
	int ret = -1;

	// Ignores it if the client is already closed.

	if (server->client_fd == -1)
	{
		return;
	}

	// Logs that we're disconnecting from the client.

	printf("fn_eth_com__disconnect_client(): disconnecting client\r\n");

	// Closes the client.

	ret = close(server->client_fd);

	// When we fail to close the client, just print an error.

	if (ret == -1) {
		fprintf(stderr, "Failed to close client socket, err no: %d\r\n", errno);
	}
}

static void fn_eth_com__read_task(void *_server) {
	st_eth_com_server_state_t *server = (st_eth_com_server_state_t*) _server;
	st_safe_area_t safe_area;
	int ret = -1;

	// Immediately suspends the task since we've not accepted any client sockets yet.

	osThreadSuspend(server->reading_task);

	// Stays in an infinite loop to read.

	while (true) {
		// Reads the incoming transaction size (is 4 bytes).

		st_transaction_t *transaction = fake_heap_calloc(
				server->transaction_heap);

		// Reads the transaction header.

		printf("fn_eth_com__read_task(): reading transaction header\r\n");

		ret = read(server->client_fd, &transaction->header,
				sizeof(st_transaction_header_t));

		// Deals with an error.

		if (ret == -1) {
			fprintf(stderr,
					"fn_eth_com__read_task(): error while reading header, err no: %d\r\n",
					errno);
		}

		// Deals with an EOS.

		if (ret == 0) {
			printf(
					"fn_eth_com__read_task(): received EOS while reading header\r\n");
		}

		// Deals with both an EOS and an error.

		if (ret == -1 || ret == 0) {
			// Frees the transaction from the heap.

			fn_safe_area_enter(&safe_area);

			fake_heap_free(server->transaction_heap, transaction);

			fn_safe_area_leave(&safe_area);

			// Suspends the current thread and resumes the server task.

			osThreadSuspend(server->reading_task);
			osThreadResume(server->server_task);

			// Starts a new iteration.

			continue;
		}

		// Prints that we've read the transaction header.

		printf(
				"fn_eth_com__read_task(): read transaction header, size: %u, expecting response: %u\r\n",
				(unsigned int) transaction->header.command_size,
				(unsigned int) (transaction->header.flags & 0x1));

		// Makes sure that the transaction command size is not too large.

		if (transaction->header.command_size > TRANSACTION_COMMAND_BUFFER_SIZE) {
			fprintf(stderr,
					"fn_eth_com__read_task(): received command that will overflow buffer of size, %u\r\n",
					TRANSACTION_COMMAND_BUFFER_SIZE);

			continue;
		}

		// Read the contents of the transaction.

		ret = read(server->client_fd, transaction->command,
				transaction->header.command_size);

		// Deals with an error.

		if (ret == -1) {
			fprintf(stderr,
					"fn_eth_com__read_task(): error while reading command, err no: %d\r\n",
					errno);
		}

		// Deals with an EOS.

		if (ret == 0) {
			printf(
					"fn_eth_com__read_task(): received EOS while reading command\r\n");
		}

		// Deals with both an EOS and an error.

		if (ret == -1 || ret == 0) {
			// Frees the transaction from the heap.

			fn_safe_area_enter(&safe_area);

			fake_heap_free(server->transaction_heap, transaction);

			fn_safe_area_leave(&safe_area);

			// Suspends the current thread and resumes the server task.

			osThreadSuspend(server->reading_task);
			osThreadResume(server->server_task);

			// Starts a new iteration.

			continue;
		}

	}
}

static void fn_eth_com__write_task(void *_server) {
	st_eth_com_server_state_t *server = (st_eth_com_server_state_t*) _server;
	en_ring_buffer_error_t ring_buffer_error = EN_RING_BUFFER_ERROR__OK;
	uint8_t bytes[64];
	int res = -1, n_bytes = 0;
	st_safe_area_t safe_area;

	// Immediately suspends the task since we've not accepted any client sockets yet.

	osThreadSuspend(server->writing_task);

	while (true) {
		if (server->client_fd == -1) {
			osDelay(1000);

			continue;
		}

		// Enters the safe area.

		fn_safe_area_enter(&safe_area);

		// Gets the number of bytes to write.

		n_bytes = fn_ring_buffer_size(&server->tx_ring_buffer);

		// Don't do anything unless there's something to do.
		if (n_bytes <= 0) {
			fn_safe_area_leave(&safe_area);

			osDelay(1000);

			continue;
		}

		// Caps the number of bytes to write.

		if (n_bytes > sizeof(bytes) / sizeof(uint8_t)) {
			n_bytes = sizeof(bytes) / sizeof(uint8_t);
		}

		// Reads the bytes from the ring buffer.

		ring_buffer_error = fn_ring_buffer_read_bytes(&server->tx_ring_buffer,
				bytes, n_bytes);

		// Makes sure that the reading from the ring buffer went okay.

		if (ring_buffer_error != EN_RING_BUFFER_ERROR__OK) {
			fprintf(stderr,
					"Failed to read bytes from TX buffer for Ethernet communication\r\n");

			Error_Handler();
		}

		// Leaves the safe area.

		fn_safe_area_leave(&safe_area);

		// Prints that we're writing data to the client.
		printf("Writing %d bytes to the client socket.\r\n", n_bytes);

		// Writes the chunk to the client socket.

		res = write(server->client_fd, bytes, n_bytes);

		// Handles the case where we have an EOS.

		if (res == 0) {
			fprintf(stderr, "Failed to write bytes to client, end of file\r\n");
		}

		// Handles the case where we have an error.

		if (res == -1) {
			fprintf(stderr, "Failed to write bytes to client, err no: %d\r\n",
					errno);
		}

		// Handles both errors.
		if (res == 0 || res == -1)
		{
			// Suspends the current thread and resumes the server task.

			osThreadSuspend(server->writing_task);
			osThreadResume(server->server_task);

			// Continues.

			continue;
		}
	}
}

/**
 * Creates the server socket.
 * @param server the server state.
 */
static void fn_eth_com__server_task__create_socket(st_eth_com_server_state_t *server)
{
	// Create the server socket.

	server->server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Make sure the server socket was successfully created.

	if (server->server_fd == -1) {
		fprintf(stderr, "Failed to create server socket, err no: %d\r\n",
				errno);

		Error_Handler();
	}

	server->server_created = true;
}

/**
 * Binds the server socket.
 * @param server the server state.
 */
static void fn_eth_com__server_task__bind_socket(st_eth_com_server_state_t *server)
{
	int res = -1;

	// Binds the server socket.

	res = bind(server->server_fd, (struct sockaddr* ) &server->server_addr,
			sizeof(struct sockaddr_in));

	// Makes sure that the binding was successful.

	if (res == -1) {
		fprintf(stderr, "Failed to bind server socket, err no: %d\r\n", errno);

		Error_Handler();
	}

	server->server_bound = true;
}

/**
 * Listens the server socket.
 * @param server the server state.
 */
static void fn_eth_com__server_task__listen_socket(st_eth_com_server_state_t *server)
{
	int res = -1;

	// Listens the server socket.

	res = listen(server->server_fd, 1);

	if (res == -1) {
		fprintf(stderr, "Failed to listen server socket, err no: %d\r\n",
				errno);

		Error_Handler();
	}

	server->server_listening = true;
}

/**
 * Accepts a new client socket from the server.
 * @param server the server state.
 */
static void fn_eth_com__server_task__accept_client(st_eth_com_server_state_t *server)
{
	socklen_t client_sock_addr_len = 0;

	// Prints that we're about to accept a client.

	printf("fn_eth_com__server_task__accept_client(): Accepting client\r\n");

	// Accepts a new client from the server.

	server->client_fd = accept(server->server_fd,
			(struct sockaddr* ) &server->client_addr,
			&client_sock_addr_len);

	// Makes sure that we've successfully accepted a client.

	if (server->client_fd <= 0) {
		fprintf(stderr, "Failed to accept client socket, err no: %d\r\n",
				errno);

		Error_Handler();
	}
}

/**
 * The task that accepts new clients.
 * @param _server the server task argument.
 */
static void fn_eth_com__server_task(void *_server) {
	st_eth_com_server_state_t *server = (st_eth_com_server_state_t*) _server;

	// Creates the socket, binds the socket and listens the socket.

	fn_eth_com__server_task__create_socket(server);
	fn_eth_com__server_task__bind_socket(server);
	fn_eth_com__server_task__listen_socket(server);

	// Stays in an infinite loop accepting socket clients.

	while (true) {
		// Accepts the client.

		fn_eth_com__server_task__accept_client(server);

		// Resumes the tasks.

		osThreadResume(server->writing_task);
		osThreadResume(server->reading_task);

		// Sleeps the current task.

		osThreadSuspend(server->server_task);

		// Closes the connection to the client.

		fn_eth_com__disconnect_client(server);
	}
}
