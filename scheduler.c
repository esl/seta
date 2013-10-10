/*
 *  scheduler.c
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 12/09/2013.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "scheduler.h"


void stack_depth_computation(processor_t *p, closure_t *cl, context_t *context) {
	int c1 = closure_space(cl);
	int c2 = context->allocated_ancients;
	int S1_candidate = c1 + c2;
	if (S1_candidate > p->stack_depth) {
		p->stack_depth = S1_candidate;
	}
	cl->allocated_ancients = context->allocated_ancients;
	context->allocated_ancients += c1;
}

void scheduler_execute_closure(processor_t *local_proc, closure_t *cl) {
	//printf("closure extracted: %s\n", cl->name);
	user_fun = cl->fun;
	context_t context;
	context.debug_list_arguments = NULL;
	context.is_last_thread = false;
	context.n_local_proc = local_proc->id;
	context.level = cl->level;
	context.allocated_ancients = cl->allocated_ancients;
	void *ptr = cl->args;
	if (cl->is_last_thread) {
		stop_computation = true;
	}
	closure_destroy(cl);
	user_fun(ptr, context);
}

void *scheduling_loop(void *ptr) {
	int n_local_proc = *(int *)ptr;
	processor_t *local_proc = processors[n_local_proc];
	while (1) {
		//printf("proc: %d size: %d\n", local_proc->id, local_proc->rq->size);
		processor_lock_ready_queue(local_proc);
		closure_t *cl = extract_head_from_deepest_level(local_proc->rq);
		processor_unlock_ready_queue(local_proc);
		if (cl != NULL) {
			char msg[80] = "", msg2[30] = "";
			strcat(msg, "-> closure extracted: \"");
			closure_print(cl, msg);
			sprintf(msg2, "\" from the level %d\n\n", cl->level);
			strcat(msg, msg2);
			
			//sprintf(msg, "-> closure extracted: \"%s\" from the level %d\n\n", 
			//		cl->name, cl->level);
			logger_write(local_proc->logger, msg);
			
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
			
			char msg[50] = "";
			sprintf(msg, "-> try stealing from the processor %d...", n_rand_proc);
			logger_write(local_proc->logger, msg);
			
			processor_t *remote_proc = processors[n_rand_proc];
			processor_lock_ready_queue(remote_proc);
			//printf("proc: %d ", remote_proc->id);
			//printf("%d ", remote_proc->rq->size);
			closure_t *rcl = extract_tail_from_shallowest_level(remote_proc->rq);
			processor_unlock_ready_queue(remote_proc);
			if (rcl == NULL) {
				logger_write(local_proc->logger, "failed.\n\n");
			}
			else {
				logger_write(local_proc->logger, "ok.\n\n");
				
				char msg[80] = "", msg2[30] = "";
				strcat(msg, "-> closure remotely extracted: \"");
				closure_print(rcl, msg);
				sprintf(msg2, "\" from the level %d\n\n", rcl->level);
				strcat(msg, msg2);
				logger_write(local_proc->logger, msg);
				

				scheduler_execute_closure(local_proc, rcl);
			}
		}		
	}
	
	char msg[50] = "\n\n//-------------------------------------//";
	logger_write(local_proc->logger, msg);
	sprintf(msg, "\ntotal_space: %dB\n", local_proc->total_space);
	logger_write(local_proc->logger, msg);
	sprintf(msg, "\nstack_depth: %dB\n", local_proc->stack_depth);
	logger_write(local_proc->logger, msg);
	
	//printf("\ntotal_space: %dB\n", local_proc->total_space);
	//printf("\nstack_depth: %dB\n", local_proc->stack_depth);
	
	return NULL;
}


// ---------

cont create_cont(void *arg) {
	cont c;
	c.arg = arg;
	return c;
}

void connect_cont(handle_spawn_next *hsn, cont *c) {
	c->cl = hsn->cl;
	hsn->cl->join_counter += 1;
}

void assign_cont(cont *c1, cont c2) {
	c1->cl = c2.cl;
	c1->arg = c2.arg;
	c1->debug_n_arg = c2.debug_n_arg;
}

cont copy_cont(cont c) {
	cont r;
	r.cl = c.cl;
	r.arg = c.arg;
	r.debug_n_arg = c.debug_n_arg;
	return r;
}

handle_spawn_next prepare_spawn_next(void *fun, void *args, context_t *context) {
	handle_spawn_next hsn;
	closure_t *cl = closure_create(context->spawned);
	closure_set_fun(cl, fun);
	closure_set_args(cl, args);
	cl->proc = context->n_local_proc;
	cl->level = context->level;
	cl->args_size = context->args_size;
	stack_depth_computation(processors[0], cl, context);
	cl->is_last_thread = context->is_last_thread;
	if (context->debug_list_arguments != NULL) {
		cl->debug_list_arguments = context->debug_list_arguments;
		context->debug_list_arguments = NULL;
	}
	context->is_last_thread = false;
	hsn.cl = cl;
	return hsn;
}

void spawn_next(handle_spawn_next hsn) {
	processor_t *local_proc = processors[hsn.cl->proc];
	if (hsn.cl->join_counter == 0) {
		
		//-- print to file
		char msg[80] = "", msg2[30] = "";
		strcat(msg, "-> spawn next \"");
		closure_print(hsn.cl, msg);
		sprintf(msg2, "\" to level %d\n\n", hsn.cl->level);
		strcat(msg, msg2);
		logger_write(local_proc->logger, msg);
		//--
		
		//char msg[30] = "";
		//sprintf(msg, "-> spawn next \"%s\" to level %d\n\n", hsn.cl->name, hsn.cl->level);
		//logger_write(local_proc->logger, msg);
		
		//printf("spawn next of \"%s\" to level %d\n", hsn.cl->name, hsn.cl->level);
		
		processor_lock_ready_queue(local_proc);
		post_closure_to_level(local_proc->rq, hsn.cl, hsn.cl->level);
		processor_unlock_ready_queue(local_proc);
	}
	else {
		//-- print to file
		char msg[80] = "";
		strcat(msg, "-> stall \"");
		closure_print(hsn.cl, msg);
		strcat(msg, "\"\n\n");
		logger_write(local_proc->logger, msg);
		//--
		
		//char msg[30] = "";
		//sprintf(msg, "-> stall \"%s\"\n\n", hsn.cl->name);
		//logger_write(local_proc->logger, msg);
		
		processor_lock_stalled(local_proc);
		add_to_stalled(local_proc, hsn.cl);
		processor_unlock_stalled(local_proc);
	}
	processor_evaluate_stats(local_proc);
	processor_print(local_proc);
}

void spawn(void *fun, void *args, context_t *context) {
	closure_t *cl = closure_create(context->spawned);
	closure_set_fun(cl, fun);
	closure_set_args(cl, args);
	cl->level = context->level + 1;
	cl->args_size = context->args_size;
	
	if (context->debug_list_arguments != NULL) {
		cl->debug_list_arguments = context->debug_list_arguments;
		context->debug_list_arguments = NULL;
	}
	
	processor_t *local_proc = processors[context->n_local_proc];
	stack_depth_computation(local_proc, cl, context);
	
	char msg[50] = "";
	sprintf(msg, "-> spawn \"%s\" to level %d\n\n", cl->name, cl->level);
	logger_write(local_proc->logger, msg);
	//printf("%s", msg);
	//printf("spawn of \"%s\" to level %d\n", cl->name, cl->level);
	
	processor_lock_ready_queue(local_proc);
	post_closure_to_level(local_proc->rq, cl, cl->level);
	processor_unlock_ready_queue(local_proc);
	
	
	processor_evaluate_stats(local_proc);
	processor_print(local_proc);
}

void send_argument(cont c, void *src, int size, context_t *context) {
	void *dst = c.arg;
	memcpy(dst, src, size);
	
	closure_t *cl = c.cl;
	cl->join_counter -= 1;
	
	if (cl->join_counter == 0) {
		//printf("enable closure \"%s\"\n", cl->name);
		processor_t *local_proc = processors[context->n_local_proc];
		processor_t *target_proc = processors[cl->proc];

		char msg[80] = "";
		if (local_proc == target_proc) {
			sprintf(msg, "-> enable local closure \"%s\" to level %d\n\n", 
					cl->name, cl->level);
		}
		else {
			sprintf(msg, "-> enable remote closure \"%s\" on processor %d to level %d\n\n", 
					cl->name, target_proc->id, cl->level);
		}

		logger_write(local_proc->logger, msg);
		
		processor_lock_stalled(target_proc);
		remove_from_stalled(target_proc, cl);
		processor_unlock_stalled(target_proc);
		
		processor_lock_ready_queue(local_proc);
		post_closure_to_level(local_proc->rq, cl, cl->level);
		processor_unlock_ready_queue(local_proc);

		processor_evaluate_stats(local_proc);
		processor_print(local_proc);
	}
}

void debug_send_argument(cont c, char *arg) {
	closure_t *cl = c.cl;
	if (cl->debug_list_arguments == NULL) {
		return;
	}
	char *str = (char *)dequeue_get_element(*(cl->debug_list_arguments), c.debug_n_arg);
	strcpy(str, arg);
}

debug_list_arguments_t *debug_list_arguments_new() {
	// TODO: destroy debug_list_arguments and related strings when the closure is destroyed
	debug_list_arguments_t *debug_list_arguments = dequeue_create();
	return debug_list_arguments;
}

void debug_list_arguments_add(debug_list_arguments_t *debug_list_arguments, char *str) {
	char *new_str = (char *)malloc(sizeof(char) * (strlen(str) + 1));
	strcpy(new_str, str);
	dequeue_add_tail(debug_list_arguments, new_str);
}



void start(void *fun)
{	
	time_t t;
	srand((unsigned)time(&t));
	
	char filename[] = "scheduler.txt";
	logger = logger_create(filename);
	
	for (int i=0; i<NPROC; i++) {
		processors[i] = processor_create(i);
	}
	
	closure_t *cl = closure_create("start");
	closure_set_fun(cl, fun);
	
	processor_lock_ready_queue(processors[0]);
	post_closure_to_level(processors[0]->rq, cl, 0);
	processor_lock_ready_queue(processors[0]);
	
	processor_evaluate_stats(processors[0]);
	processor_print(processors[0]);
	
	for (int i=0; i<NPROC; i++) {
		processor_start(processors[i]);
	}
	for (int i=0; i<NPROC; i++) {
		processor_wait_4_me(processors[i]);
	}
	
	
	for (int i=0; i<NPROC; i++) {
		processor_destroy(processors[i]);
	}
	
	logger_destroy(logger);
}
