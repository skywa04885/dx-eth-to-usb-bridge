/*
 * transaction;.h
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#ifndef INC_TRANSACTION_H_
#define INC_TRANSACTION_H_

#include <stdint.h>

#define TRANSACTION_COMMAND_BUFFER_SIZE 256U

struct __attribute__ (( packed )) st_transaction_header
{
	uint32_t flags;
	uint32_t command_size;
};

typedef struct st_transaction_header st_transaction_header_t;

struct __attribute__ (( packed )) st_transaction
{
	st_transaction_header_t header;
	uint8_t command[TRANSACTION_COMMAND_BUFFER_SIZE];
};

typedef struct st_transaction st_transaction_t;

#endif /* INC_TRANSACTION_H_ */
