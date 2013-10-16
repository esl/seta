/*
 *  scheduler.h
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 12/09/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "processor.h"


typedef struct _context_t {
	int level;
	char *spawned;
	int args_size;
	int allocated_ancients;
	bool is_last_thread;
	int n_local_proc;
	dequeue_t *debug_list_arguments;
} context_t;

void (*user_fun)(void *, context_t);

typedef struct _cont {
	closure_t *cl;
	void *arg;
	int debug_n_arg;
} cont;

typedef struct _handle_spawn_next {
	closure_t *cl;
} handle_spawn_next;

bool stop_computation;

#define NPROC 2

processor_t *processors[NPROC];

logger_t logger;

typedef dequeue_t debug_list_arguments_t;







cont create_cont(void *);

handle_spawn_next prepare_spawn_next(void *, void *, context_t *);

void spawn_next(handle_spawn_next);

void spawn(void *, void *, context_t *);

void send_argument(cont, void *, int, context_t *);

void debug_send_argument(cont, char *);

void scheduler_start(void *);

void connect_cont(handle_spawn_next *, cont *);

void assign_cont(cont *, cont);

cont copy_cont(cont);

debug_list_arguments_t *debug_list_arguments_new();

void debug_list_arguments_add(debug_list_arguments_t *, char *);

