/*
 *  utils.c
 *  seta
 *
 *  Created by Fabio Pricoco on 11/10/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "utils.h"

char * strcatformat(char *dest, const char *format, ...) {
	va_list arglist;
	va_start(arglist, format);
	char str[200];
	vsprintf(str, format, arglist);
	strcat(dest, str);
	return dest;
}