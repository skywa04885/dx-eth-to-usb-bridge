/*
 * disconnecting_from_client.h
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#ifndef INC_STATES_READING_COMMAND_HEADER_FROM_SOCKET_H_
#define INC_STATES_READING_COMMAND_HEADER_FROM_SOCKET_H_

#include "state_machine.h"

void fn_reading_command_header_from_socket__entry(st_state_machine_t *sm);

void fn_reading_command_header_from_socket__loop(st_state_machine_t *sm);

void fn_reading_command_header_from_socket__exit(st_state_machine_t *sm);

#endif /* INC_STATES_READING_COMMAND_HEADER_FROM_SOCKET_H_ */
