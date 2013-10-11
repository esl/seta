/*
 *  closure.c
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 10/09/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "closure.h"
#include "string.h"

int closure_id = 0;

closure_t * closure_create(char *fun_name) {
	closure_t *cl = (closure_t *) malloc(sizeof(closure_t));
	char *name = (char *) malloc(sizeof(char) * NAMELEN);
	strcpy(name, fun_name);
	cl->id = ++closure_id;
	cl->name = name;
	cl->proc = 0;
	cl->fun = NULL;
	cl->level = 0;
	cl->join_counter = 0;
	cl->args = NULL;
	cl->args_size = 0;
	cl->allocated_ancients = 0;
	cl->is_last_thread = false;
	cl->debug_list_arguments = NULL;
	return cl;
}

void closure_destroy_argument(char *arg) {
	free(arg);
}

void closure_destroy(closure_t *cl) {
	// TODO destroy cl->debug_list_arguments
	if (cl->debug_list_arguments != NULL) {
		dequeue_foreach(&closure_destroy_argument, cl->debug_list_arguments);
		dequeue_destroy(cl->debug_list_arguments);
	}
	free(cl->name);
	free(cl);
}

void closure_set_fun(closure_t *cl, void *fun) {
	cl->fun = fun;
}

void closure_set_args(closure_t *cl, void *args) {
	cl->args = args;
}

void debug_list_arguments_callback(void *ptr, void *acc_in, int index) {
	char *str = (char *)ptr;
	char *res = (char *)acc_in;
	strcat(res, str);
	strcat(res, ",");
}

void closure_print(closure_t *cl, char *acc) {
	strcat(acc, cl->name);
	if (cl->debug_list_arguments != NULL) {
		char str[100] = "(";
		dequeue_fold(&debug_list_arguments_callback, str, cl->debug_list_arguments);
		strncat(acc, str, strlen(str) - 1);
		strcat(acc, ")");
	}
}

void closure_print_callback(void *ptr, void *acc_in, int index) {
	char *res = (char *)acc_in;
	closure_t *cl = (closure_t *)ptr;
	strcat(res, cl->name);
	if (cl->debug_list_arguments != NULL) {
		char str[100] = "(";
		dequeue_fold(&debug_list_arguments_callback, str, cl->debug_list_arguments);
		strncat(res, str, strlen(str) - 1);
		strcat(res, ")");
	}
	strcat(res, " ");
	
}

int closure_space(void *cl) {
	return sizeof(closure_t) + sizeof(char) * NAMELEN + ((closure_t *)cl)->args_size;
}

void closure_space_callback(void *cl, void *acc_in, int index) {
	*(int *)acc_in += closure_space((closure_t *)cl);
}


