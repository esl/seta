/*
 *  utils.c
 *  seta
 *
 *  Created by Fabio Pricoco on 11/10/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "seta_internal.h"

msg_t msg_new() {
	char *empty_string = (char *)malloc(sizeof(char));
	*empty_string = '\0';
	return empty_string;
}

msg_t msg_new_from(char *str) {
	int dim_new = sizeof(char) * ((int)strlen(str) + 1);
	char *new_str = (char *)malloc(dim_new);
	memset(new_str, '\0', dim_new);
	strncpy(new_str, str, dim_new - 1);
	return new_str;
}

void msg_destroy(msg_t msg) {
	free(msg);
}

void msg_ncat(msg_t *dest, char *src, int n) {
	int dim_res = (int)strlen(src) + (int)strlen(*dest) + 1;
	char *res = malloc(dim_res);
	memset(res, '\0', dim_res);
	strcat(res, *dest);
	strncat(res, src, n);
	free(*dest);
	*dest = res;
}

void msg_cat(msg_t *dest, const char *format, ...) {
	va_list arglist;
	va_start(arglist, format);
	char src[500];
	vsprintf(src, format, arglist);
	//printf("%s\n", format);
	int dim_res = (int)strlen(src) + (int)strlen(*dest) + 1;
	char *res = malloc(dim_res);
	memset(res, '\0', dim_res);
	strcpy(res, *dest);
	strcat(res, src);
	free(*dest);
	*dest = res;
	//printf("'%s'\n", *dest);
}
