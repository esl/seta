/*
 *  processor.h
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 07/10/2013.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "ready_queue.h"
#include <pthread.h>
#include <string.h>


typedef struct _processor {
	int id;
	ready_queue *rq;
	dequeue_t *stalled;
	int cur_space;
	int total_space;
	int stack_depth;
	pthread_t thread;
	pthread_mutex_t rq_mutex;
	pthread_mutex_t stalled_mutex;
	logger_t logger;  
} processor_t;






processor_t * processor_create(int);

void processor_destroy(processor_t *);

void processor_print(processor_t *);

void processor_evaluate_stats(processor_t *);

void processor_start(processor_t *);

void processor_wait_4_me(processor_t *);

void add_to_stalled(processor_t *, closure_t *);

void remove_from_stalled(processor_t *, closure_t *);

extern void *scheduling_loop(void *);

void processor_lock_ready_queue(processor_t *);

void processor_unlock_ready_queue(processor_t *);

void processor_lock_stalled(processor_t *);

void processor_unlock_stalled(processor_t *);