/*
 *  utils.c
 *  seta
 *
 *  Created by Fabio Pricoco on 11/10/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "utils.h"
#include <stdlib.h>

/*
msg_t msg_new();

void msg_destroy(msg_t);

void istrcat(msg_t, char *);

void istrncat(msg_t, char *, int);

void istrcatf(msg_t, const char *, ...);
*/

/*char * strcatformat(char *dest, const char *format, ...) {
	va_list arglist;
	va_start(arglist, format);
	char str[200];
	vsprintf(str, format, arglist);
	strcat(dest, str);
	return dest;
}*/


msg_t msg_new() {
	char *empty_string = (char *)malloc(sizeof(char));
	*empty_string = '\0';
	return empty_string;
}

void msg_destroy(msg_t msg) {
	free(msg);
}

void istrcat(msg_t *dest, char *src) {
	char *res = malloc(strlen(src) + strlen(*dest) + 1);
	memset(res, '\0', sizeof(res));
	strcat(res, *dest);
	strcat(res, src);
	free(*dest);
	*dest = res;
}

void istrncat(msg_t *dest, char *src, int n) {
	int dim_res = strlen(src) + strlen(*dest) + 1;
	char *res = malloc(dim_res);
	memset(res, '\0', dim_res);
	strcat(res, *dest);
	strncat(res, src, n);
	free(*dest);
	*dest = res;
}

void istrcatf(msg_t *dest, const char *format, ...) {
	va_list arglist;
	va_start(arglist, format);
	char str[200];
	vsprintf(str, format, arglist);
	istrcat(dest, str);	
}

