/*
 *  logger.c
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 08/10/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "seta_internal.h"
#include "stdio.h"

logger_t logger_create(char *filename) {
	logger_t logger;
	logger.fp = fopen(filename, "w");
    if (logger.fp == NULL) {
        printf("error opening the file\n");
        exit(-1);
    }
	return logger;
}

void logger_destroy(logger_t logger) {
	fclose(logger.fp);
}

void logger_write(logger_t logger, const char *format, ...) {
	va_list arglist;
	va_start(arglist, format);
	vfprintf(logger.fp, format, arglist);
	fflush(logger.fp);
}
