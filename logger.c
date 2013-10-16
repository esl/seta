/*
 *  logger.c
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 08/10/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "logger.h"

logger_t logger_create(char *filename) {
	logger_t logger;
	logger.fp = fopen(filename, "w");
	return logger;
}

void logger_destroy(logger_t logger) {
	fclose(logger.fp);
}

void logger_write(logger_t logger, char *str) {
	fprintf(logger.fp, "%s", str);
	fflush(logger.fp);
}

void logger_write_format(logger_t logger, const char *format, ...) {
	va_list arglist;
	va_start(arglist, format);
	vfprintf(logger.fp, format, arglist);
	fflush(logger.fp);
}
