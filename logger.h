/*
 *  logger.h
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 08/10/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdarg.h>
#include "utils.h"

typedef struct _logger {
	FILE *fp;
} logger_t;



logger_t logger_create(char *);

void logger_destroy(logger_t);

void logger_write(logger_t, char *str);

void logger_write_format(logger_t logger, const char *format, ...);