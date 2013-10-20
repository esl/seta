//
//  scheduler.c
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include "seta_internal.h"
#include <string.h>

#define NPROC 4

bool stop_computation = false;

processor_t *processors[NPROC];

seta_cont_t seta_cont_create(void *arg, handle_spawn_next_t hsn) {
	seta_cont_t cont;
	cont.closure = hsn.closure;
	cont.arg = arg;
	((closure_t *)hsn.closure)->join_counter += 1;
	return cont;
}

handle_spawn_next_t seta_prepare_spawn_next(void *fun, void *args, seta_context_t *context) {
	handle_spawn_next_t hsn;
	closure_t *cl = closure_create();
	closure_set_fun(cl, fun);
	closure_set_args(cl, args);
	cl->level = context->level;
	cl->is_last_thread = context->is_last_thread;
	context->is_last_thread = false;
	hsn.closure = (void *)cl;
	return hsn;
}

void seta_spawn_next(handle_spawn_next_t hsn) {
}

void seta_spawn(void *fun, void *args, seta_context_t *context) {
	closure_t *cl = closure_create();
	closure_set_fun(cl, fun);
	closure_set_args(cl, args);
	cl->level = context->level + 1;
	
	processor_t *local_proc = processors[context->n_local_proc];
	
	processor_lock_ready_queue(local_proc);
	ready_queue_post_closure_to_level(local_proc->rq, cl, cl->level);
	processor_unlock_ready_queue(local_proc);
}

void seta_send_argument(seta_cont_t cont, void *src, int size, seta_context_t *context) {
	closure_t *cl = (closure_t *)cont.closure;
	processor_t *local_proc = processors[context->n_local_proc];
	
	closure_lock(cl);
	void *dst = cont.arg;
	memcpy(dst, src, size);
	
	cl->join_counter -= 1;
	if (cl->join_counter == 0) {
		processor_lock_ready_queue(local_proc);
		ready_queue_post_closure_to_level(local_proc->rq, cl, cl->level);
		processor_unlock_ready_queue(local_proc);
	}
	closure_unlock(cl);
	
}

void scheduler_execute_closure(processor_t *local_proc, closure_t *cl) {
	if (cl->is_last_thread) {
		stop_computation = true;
	}
	seta_context_t context;
	context.is_last_thread = false;
	context.n_local_proc = local_proc->id;
	context.level = cl->level;
	void (*user_fun)(void *, seta_context_t) = cl->fun;
	void *ptr = cl->args;
	closure_destroy(cl);
	user_fun(ptr, context);
}

void *scheduler_scheduling_loop(void *ptr) {
	int n_local_proc = *(int *)ptr;
	processor_t *local_proc = processors[n_local_proc];
	while (1) {
		processor_lock_ready_queue(local_proc);
		closure_t *cl = ready_queue_extract_head_from_deepest_level(local_proc->rq);
		processor_unlock_ready_queue(local_proc);
		if (cl != NULL) {
			scheduler_execute_closure(local_proc, cl);
			
			if (stop_computation) {
				break;
			}
		}
		else {
			if (stop_computation) {
				break;
			}
			int n_rand_proc = 0;
			do {
				n_rand_proc = rand() % NPROC;
			} while (n_rand_proc == n_local_proc);
            
            processor_t *remote_proc = processors[n_rand_proc];
            
            processor_lock_ready_queue(remote_proc);
            closure_t *rcl = ready_queue_extract_tail_from_shallowest_level(remote_proc->rq);
            processor_unlock_ready_queue(remote_proc);
            
            if (rcl != NULL) {
                scheduler_execute_closure(local_proc, rcl);
            }
        }
	}
	return NULL;
}

void seta_start(void *fun)
{
	time_t t;
	srand((unsigned)time(&t));
	
	for (int i=0; i<NPROC; i++) {
		processors[i] = processor_create(i);
	}
	
	closure_t *cl = closure_create("start");
	closure_set_fun(cl, fun);
	
	processor_lock_ready_queue(processors[0]);
	ready_queue_post_closure_to_level(processors[0]->rq, cl, 0);
	processor_unlock_ready_queue(processors[0]);
	
	for (int i=0; i<NPROC; i++) {
		processor_start(processors[i]);
	}
	for (int i=0; i<NPROC; i++) {
		processor_wait_4_me(processors[i]);
	}
	
	for (int i=0; i<NPROC; i++) {
		processor_destroy(processors[i]);
	}
}
