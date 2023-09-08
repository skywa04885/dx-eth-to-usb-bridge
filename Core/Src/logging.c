/*
 * logging.c
 *
 *  Created on: Sep 8, 2023
 *      Author: luke
 */

#include "logging.h"

#include <stdio.h>
#include <stdarg.h>

void _mlog(const char *filename, const char *function, const int line, const char *fmt, ...)
{
	printf("%s %s %d : ", filename, function, line);

	va_list args;

	va_start(args, fmt);

	vprintf(fmt, args);

	va_end(args);

	printf("\r\n");
}
