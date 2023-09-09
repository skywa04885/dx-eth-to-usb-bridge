/*
 * statemachine.h
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#ifndef INC_STATE_MACHINE_H_
#define INC_STATE_MACHINE_H_

#include <cmsis_os2.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "usbh_core.h"

#define ST_STATE_MACHINE__COMMAND_PAYLOAD_SIZE 256U

enum en_state_machine_state
{
	EN_STATE_MACHINE_STATE__CONNECTING_TO_CLIENT = 0,
	EN_STATE_MACHINE_STATE__READING_COMMAND_HEADER_FROM_SOCKET = 1,
	EN_STATE_MACHINE_STATE__READING_COMMAND_PAYLOAD_FROM_SOCKET = 2,
	EN_STATE_MACHINE_STATE__WRITING_COMMAND_TO_USB = 3,
	EN_STATE_MACHINE_STATE__READING_RESPONSE_FROM_USB = 4,
	EN_STATE_MACHINE_STATE__WRITING_RESPONSE_TO_SOCKET = 5,
	EN_STATE_MACHINE_STATE__CONNECTING_TO_USB = 6,
	EN_STATE_MACHINE_STATE__DISCONNECTING_FROM_USB = 7,
};

typedef enum en_state_machine_state en_state_machine_state_t;

const char *fn_state_machine_state_to_string(en_state_machine_state_t state);

typedef struct {
	uint32_t lastYellowLedBlink;
} DX_StateMachine_ConnectingToUSBState_t;

typedef struct {
	uint32_t lastGreenBlinkTick;
} DX_StateMachine_ConnectingToClientState_t;

struct st_reading_command_header_from_socket_state
{
	int32_t read_rc;
	uint32_t n_bytes_read;
};

typedef struct st_reading_command_header_from_socket_state st_reading_command_header_from_socket_state_t;

struct st_reading_command_payload_from_socket_state
{
	int32_t read_rc;
	uint32_t n_bytes_read;
};

typedef struct st_reading_command_payload_from_socket_state st_reading_command_payload_from_socket_state_t;

struct st_state_machine_client
{
	struct sockaddr_in addr;
	socklen_t socklen;
	int32_t fd;
};

typedef struct st_state_machine_client st_state_machine_client_t;

struct st_state_machine_server
{
	struct sockaddr_in addr;
	int32_t fd;
};

typedef struct st_state_machine_server st_state_machine_server_t;

struct st_state_machine
{
	osMessageQueueId_t usbEventQueue;
	/* -------------- */
	st_state_machine_client_t client;
	st_state_machine_server_t server;
	/* -------------- */
	en_state_machine_state_t previousState;
	en_state_machine_state_t current_state;
	en_state_machine_state_t next_state;
	/* -------------- */
	uint32_t command_size;
	uint8_t command_payload[ST_STATE_MACHINE__COMMAND_PAYLOAD_SIZE];
	/* -------------- */
	st_reading_command_header_from_socket_state_t reading_command_header_from_socket_state;
	st_reading_command_payload_from_socket_state_t reading_command_payload_from_socket_state;
	DX_StateMachine_ConnectingToUSBState_t connectingToUSBState;
	DX_StateMachine_ConnectingToClientState_t connectingToClientState;
};

typedef struct st_state_machine st_state_machine_t;

void fn_state_machine_start(st_state_machine_t *sm);

void fn_state_machine_init(st_state_machine_t *sm);

void fn_state_machine_loop(st_state_machine_t *sm);

#endif /* INC_STATE_MACHINE_H_ */
