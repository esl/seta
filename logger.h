/*
 *  logger.h
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 08/10/2013.
 *  Copyright 2013 Erlang Solutions. All rights reserved.
 *
 */


#include <stdio.h>

typedef struct _logger {
	FILE *fp;
} logger_t;



logger_t logger_create(char *);

void logger_destroy(logger_t);

void logger_write(logger_t, char *str);