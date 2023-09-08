/*
 * connecting_to_client.h
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#ifndef INC_STATES_CONNECTING_TO_CLIENT_H_
#define INC_STATES_CONNECTING_TO_CLIENT_H_

#include "state_machine.h"

void fn_connecting_to_client__entry(st_state_machine_t *sm);

void fn_connecting_to_client__loop(st_state_machine_t *sm);

void fn_connecting_to_client__exit(st_state_machine_t *sm);

#endif /* INC_STATES_CONNECTING_TO_CLIENT_H_ */
