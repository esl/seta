/*
 *  processor.c
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 07/10/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "processor.h"

processor_t * processor_create(int id) {
	processor_t *proc = (processor_t *)malloc(sizeof(processor_t));
	proc->id = id;
	proc->rq = ready_queue_create();
	proc->stalled = dequeue_create();
	proc->cur_space = 0;
	proc->total_space = 0;
	proc->rq_mutex; //= PTHREAD_MUTEX_INITIALIZER;
	char filename[15];
	sprintf(filename, "proc_%d.txt", id);
	proc->logger = logger_create(filename);
	return proc;
}

void processor_destroy(processor_t *p) {
	ready_queue_destroy(p->rq);
	dequeue_destroy(p->stalled);
	logger_destroy(p->logger);
	free(p);
}

void processor_print(processor_t *p) {
	msg_t msg = msg_new();
	istrcatf(&msg, "----------processor %d------------\n", p->id);
	ready_queue_str(&msg, p->rq);
	istrcat(&msg, "stalled: ");
	
	dequeue_fold(&closure_str_cb, &msg, p->stalled);
	
	
	istrcatf(&msg, "\nclosures_allocated: %dB", p->cur_space);
	
	istrcat(&msg, "\n---------------------------------\n\n");
	
	logger_write(p->logger, msg);
	
	msg_destroy(msg);

}

int processor_space(processor_t *p) {
	int c1 = ready_queue_space(p->rq);
	int c2 = 0;
	dequeue_fold(&closure_space_cb, &c2, p->stalled);
	return c1 + c2;
}

void processor_evaluate_stats(processor_t *p) {
	int cs = processor_space(p);
	p->cur_space = cs;
	if (cs > p->total_space) {
		p->total_space = cs;
	} 
}

void processor_start(processor_t *proc) {
	pthread_create(&proc->thread, NULL, scheduling_loop, &proc->id);
}

void processor_wait_4_me(processor_t *proc) {
	pthread_join(proc->thread, NULL);
}

void add_to_stalled(processor_t *p, closure_t *cl) {
	dequeue_add_head(p->stalled, cl);
}


void remove_from_stalled(processor_t *p, closure_t *cl) {
	dequeue_remove_element_by_value(p->stalled, cl);
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


