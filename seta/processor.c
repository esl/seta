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

void processor_create_info(processor_t *proc) {
	proc->stalled = dequeue_create();
	proc->cur_space = 0;
	proc->total_space = 0;
	proc->stack_depth = 0;
}

void processor_destroy_info(processor_t *proc) {
	dequeue_destroy(proc->stalled);
	pthread_mutex_destroy(&proc->stalled_mutex);
}

void processor_destroy(processor_t *proc) {
	ready_queue_destroy(proc->rq);
	pthread_mutex_destroy(&proc->rq_mutex);
	free(proc);
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

void processor_lock_stalled(processor_t *proc) {
	pthread_mutex_lock(&proc->stalled_mutex);
}

void processor_unlock_stalled(processor_t *proc) {
	pthread_mutex_unlock(&proc->stalled_mutex);
}

void processor_add_to_stalled(processor_t *p, closure_t *cl) {
	dequeue_add_head(p->stalled, cl);
}

void processor_remove_from_stalled(processor_t *p, closure_t *cl) {
	dequeue_remove_element_by_value(p->stalled, cl);
}

int processor_space(processor_t *p) {
	int c1 = ready_queue_space(p->rq);
	int c2 = 0;
	dequeue_fold(&closure_space_cb, &c2, p->stalled);
	return c1 + c2;
}

void processor_computate_space(processor_t *p) {
	int cs = processor_space(p);
	p->cur_space = cs;
	if (cs > p->total_space) {
		p->total_space = cs;
	} 
}
