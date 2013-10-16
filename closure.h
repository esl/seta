/*
 *  closure.h
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 10/09/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "dequeue.h"
#include <pthread.h>

#define NAMELEN 10

typedef struct _closure {
	int id;
	char *name;
	int proc;
	int level;
	int join_counter;
	void *fun;
	void *args;
	int args_size;
	int allocated_ancients;
	bool is_last_thread;
	dequeue_t *list_arguments;
	pthread_mutex_t mutex;
} closure_t;



closure_t * closure_create(char *);

void closure_destroy(closure_t *);

void closure_set_fun(closure_t *, void *);

void closure_set_args(closure_t *, void *);

int closure_space(void *);

void closure_space_cb(void *, void *, int);

//void closure_print(closure_t *, char *);

void closure_set_list_arguments(closure_t *, dequeue_t *);

dequeue_t *closure_create_list_arguments();

void closure_list_arguments_add(dequeue_t *, char *);

void closure_str(char **, closure_t *);

void closure_str_cb(void *, void **, int);

void closure_lock(closure_t *);

void closure_unlock(closure_t *);