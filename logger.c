/*
 *  logger.c
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 08/10/2013.
 *  Copyright 2013 Erlang Solutions. All rights reserved.
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
	//printf("%s", str);
	fflush(logger.fp);
}