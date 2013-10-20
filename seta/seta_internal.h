//
//  seta_internal.c
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include "seta.h"
#include <pthread.h>
#include "dequeue.h"

typedef struct _closure {
	char *name;
	int level;
	int join_counter;
	void *fun;
	void *args;
	bool is_last_thread;
	pthread_mutex_t mutex;
} closure_t;
closure_t * closure_create();
void closure_destroy(closure_t *);
void closure_set_fun(closure_t *, void *);
void closure_set_args(closure_t *, void *);
void closure_lock(closure_t *);
void closure_unlock(closure_t *);

typedef dequeue_t ready_queue_t;

typedef struct _processor {
	int id;
	ready_queue_t *rq;
	pthread_t thread;
	pthread_mutex_t rq_mutex;
} processor_t;
processor_t * processor_create(int);
void processor_destroy(processor_t *);
void processor_start(processor_t *);
void processor_wait_4_me(processor_t *);
void processor_lock_ready_queue(processor_t *);
void processor_unlock_ready_queue(processor_t *);

ready_queue_t * ready_queue_create();
void ready_queue_destroy(dequeue_t *rq);
closure_t * ready_queue_extract_head_from_deepest_level(ready_queue_t *);
closure_t * ready_queue_extract_tail_from_shallowest_level(ready_queue_t *);
void ready_queue_post_closure_to_level(ready_queue_t *, closure_t *, int);

void *scheduler_scheduling_loop(void *);
