/*
 * eth_com.h
 *
 *  Created on: Sep 7, 2023
 *      Author: luke
 */

#ifndef INC_ETH_COM_H_
#define INC_ETH_COM_H_

#include<stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <cmsis_os.h>

#include "ring_buffer.h"
#include "fake_heap.h"
#include "transaction.h"

struct st_eth_com_server_state
{
	/* Tasks */
	osThreadId_t reading_task;
	osThreadId_t writing_task;
	osThreadId_t server_task;
	/* Fake Heaps */
	fake_heap_t *transaction_heap;
	/* Buffers */
	st_ring_buffer_t tx_ring_buffer;
	/* Client */
	struct sockaddr_in client_addr;
	int client_fd;
	bool client_connected;
	/* Server */
	struct sockaddr_in server_addr;
	int server_fd;
	bool server_created;
	bool server_listening;
	bool server_bound;
};

typedef struct st_eth_com_server_state st_eth_com_server_state_t;

void fn_eth_com_server_init(st_eth_com_server_state_t *restrict server,
		uint8_t *restrict tx_buffer, uint32_t tx_buffer_capacity,
		uint8_t *restrict rx_buffer, uint32_t rx_buffer_capacity);

#endif /* INC_ETH_COM_H_ */
