/*
 *  seta.c
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 03/10/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "seta.h"

cont seta_create_cont(void *arg) {
	return create_cont(arg);
}

handle_spawn_next seta_prepare_spawn_next(void *fun, void *args, context_t *context) {
	return prepare_spawn_next(fun, args, context);
}

void seta_spawn_next(handle_spawn_next hsn) {
	spawn_next(hsn);
}

void seta_spawn(void *fun, void *args, context_t *context) {
	spawn(fun, args, context);
}

void seta_send_argument(cont c, void *src, int size, context_t *context) {
	send_argument(c, src, size, context);
}

void seta_start(void *fun) {
	scheduler_start(fun);
}

void seta_connect_cont(handle_spawn_next *hsn, cont *c) {
	connect_cont(hsn, c);
}

void seta_assign_cont(cont *c1, cont c2) {
	assign_cont(c1, c2);
}

cont seta_copy_cont(cont c) {
	return copy_cont(c);
}