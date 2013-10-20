//
//  processor.c
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include "seta_internal.h"

processor_t * processor_create(int id) {
	processor_t *proc = (processor_t *)malloc(sizeof(processor_t));
	proc->id = id;
	proc->rq = ready_queue_create();
	pthread_mutex_init(&proc->rq_mutex, NULL);
	return proc;
}

void processor_destroy(processor_t *p) {
	ready_queue_destroy(p->rq);
	pthread_mutex_destroy(&p->rq_mutex);
	free(p);
}

void processor_start(processor_t *proc) {
	pthread_create(&proc->thread, NULL, scheduler_scheduling_loop, &proc->id);
}

void processor_wait_4_me(processor_t *proc) {
	pthread_join(proc->thread, NULL);
}

void processor_lock_ready_queue(processor_t *proc) {
	pthread_mutex_lock(&proc->rq_mutex);
}

void processor_unlock_ready_queue(processor_t *proc) {
	pthread_mutex_unlock(&proc->rq_mutex);
}



