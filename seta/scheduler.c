//
//  scheduler.c
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include "seta_internal.h"

#define NPROC 4

bool stop_computation = false;

processor_t *processors[NPROC];

bool info = false;

//TODO handle race condition on S1
int S1 = 0;
dequeue_t *S1_list = NULL;

void seta_enable_info() {
	info = true;
}

void scheduler_computate_processor_space(processor_t *local_proc) {
	processor_lock_ready_queue(local_proc);
	processor_lock_stalled(local_proc);
	processor_computate_space(local_proc);
	//printf("memory allocated on processor %d: %d\n", local_proc->id, local_proc->cur_space);
	processor_unlock_stalled(local_proc);
	processor_unlock_ready_queue(local_proc);	
}

seta_cont_t seta_cont_create(void *arg, seta_handle_spawn_next_t hsn) {
	seta_cont_t cont;
	cont.closure = hsn.closure;
	cont.arg = arg;
	((closure_t *)hsn.closure)->join_counter += 1;
	return cont;
}

void * allocated_ancient_copy(void *allocated_ancient) {
	return (void *)msg_new_from((char *)allocated_ancient);
}

dequeue_t * allocated_ancient_list_copy(dequeue_t *allocated_ancient_list) {
	return dequeue_copy(&allocated_ancient_copy, allocated_ancient_list);
}

void allocated_ancient_destroy(char *allocated_ancient) {
	free(allocated_ancient);
}

void allocated_ancient_list_destroy(dequeue_t *allocated_ancient_list) {
	if (allocated_ancient_list != NULL) {
		dequeue_foreach(&allocated_ancient_destroy, allocated_ancient_list);
		dequeue_destroy(allocated_ancient_list);
	}
}

void stack_depth_computation(processor_t *proc, closure_t *closure, seta_context_t *context) {
	dequeue_t *allocated_ancient_list = (dequeue_t *)context->allocated_ancient_list;
	closure->allocated_ancient_list = allocated_ancient_list_copy(allocated_ancient_list);
	msg_t new_ancient = msg_new();
	msg_cat(&new_ancient, "%d", closure->id);
	dequeue_add_tail(allocated_ancient_list, new_ancient);
	closure->allocated_ancients = context->allocated_ancients;
	int c1 = closure_space(closure);
	int c2 = context->allocated_ancients;
	int S1_candidate = c1 + c2;
	if (S1_candidate > S1) {
		S1 = S1_candidate;
		if (S1_list != NULL) {
			allocated_ancient_list_destroy(S1_list);
		}
		S1_list = allocated_ancient_list_copy(allocated_ancient_list);
	}
	context->allocated_ancients += c1;
	
}

seta_handle_spawn_next_t seta_prepare_spawn_next(void *fun, void *args, seta_context_t *context) {
	seta_handle_spawn_next_t hsn;
	closure_t *closure = closure_create();
	if (info) {
		closure_create_info(closure, context->spawned);
	}
	closure_set_fun(closure, fun);
	closure_set_args(closure, args);
	closure->level = context->level;
	closure->is_last_thread = context->is_last_thread;
	context->is_last_thread = false;
	if (info) {
		closure->args_size = context->args_size;
		closure->proc = context->n_local_proc;
		if (context->arg_name_list != NULL) {
			closure->arg_name_list = (dequeue_t *)context->arg_name_list;
			context->arg_name_list = NULL;
		}
		processor_t *local_proc = processors[context->n_local_proc];
		stack_depth_computation(local_proc, closure, context);
		graph_spawn_next(closure, context);
	}
	hsn.closure = (void *)closure;
	return hsn;
}

void seta_spawn_next(seta_handle_spawn_next_t hsn) {
	if (info) {
		processor_t *local_proc = processors[((closure_t *)hsn.closure)->proc];
		processor_lock_stalled(local_proc);
		processor_add_to_stalled(local_proc, hsn.closure);
		processor_unlock_stalled(local_proc);
		
		if (info) {
			scheduler_computate_processor_space(local_proc);
		}
	}
}

void seta_spawn(void *fun, void *args, seta_context_t *context) {
	processor_t *local_proc = processors[context->n_local_proc];
	closure_t *closure = closure_create();
	if (info) {
		closure_create_info(closure, context->spawned);
	}
	closure_set_fun(closure, fun);
	closure_set_args(closure, args);
	closure->level = context->level + 1;
	if (info) {
		closure->args_size = context->args_size;
		if (context->arg_name_list!= NULL) {
			closure->arg_name_list = (dequeue_t *)context->arg_name_list;
			context->arg_name_list = NULL;
		}
		stack_depth_computation(local_proc, closure, context);
		graph_spawn(closure, context);
		scheduler_computate_processor_space(local_proc);
	}
	
	processor_lock_ready_queue(local_proc);
	ready_queue_post_closure_to_level(local_proc->rq, closure, closure->level);
	processor_unlock_ready_queue(local_proc);
}

void seta_send_argument(seta_cont_t cont, void *src, int size, seta_context_t *context) {
	closure_t *closure = (closure_t *)cont.closure;
	processor_t *local_proc = processors[context->n_local_proc];
	
	void *dst = cont.arg;
	memcpy(dst, src, size);
	
    bool to_post = false;
	closure_lock(closure);
	closure->join_counter -= 1;
	if (closure->join_counter == 0) {
		to_post = true;
	}
	closure_unlock(closure);
	
    if (info) {
        graph_send_argument(closure, context);
    }
    
	if (to_post) {
		if (info) {
			processor_t *target_proc = processors[closure->proc];
			processor_lock_stalled(target_proc);
			processor_remove_from_stalled(target_proc, closure);
			processor_unlock_stalled(target_proc);
			
			scheduler_computate_processor_space(local_proc);
			graph_send_argument_enable(closure, context);
		}
		processor_lock_ready_queue(local_proc);
		ready_queue_post_closure_to_level(local_proc->rq, closure, closure->level);
		processor_unlock_ready_queue(local_proc);
	}
}

void seta_send_arg_name(seta_cont_t cont, char *arg_name) {
	closure_t *closure = cont.closure;
	if (closure->arg_name_list == NULL) {
		printf("error: arg sent to no existing closure");
		return;
	}
	char **arg_name_ptr = (char **)dequeue_get_element(*(closure->arg_name_list), cont.n_arg);
	msg_destroy(*arg_name_ptr);
	*arg_name_ptr = msg_new_from(arg_name);
}

seta_arg_name_list_t seta_arg_name_list_new() {
	dequeue_t *arg_name_list = dequeue_create();
	seta_arg_name_list_t ptr = (void *)arg_name_list;
	return ptr;
}

void seta_arg_name_list_add(seta_arg_name_list_t ptr, char *str) {	
	dequeue_t *arg_name_list = (dequeue_t *)ptr;
	char **str_ptr = (char **)malloc(sizeof(char *));
	msg_t new_str = msg_new_from(str);
	*str_ptr = new_str;
	dequeue_add_tail(arg_name_list, str_ptr);
}

void scheduler_execute_closure(processor_t *local_proc, closure_t *closure) {
	if (closure->is_last_thread) {
		stop_computation = true;
	}
	seta_context_t context;
	if (info) {
		context.arg_name_list = NULL;
		context.allocated_ancients = closure->allocated_ancients;
		context.allocated_ancient_list = (void *)allocated_ancient_list_copy(
														closure->allocated_ancient_list);
		context.closure_id = closure->id;
		graph_execute(closure, local_proc->id);
	}
	context.is_last_thread = false;
	context.n_local_proc = local_proc->id;
	context.level = closure->level;
	void (*user_fun)(void *, seta_context_t) = closure->fun;
	void *ptr = closure->args;
	if (info) {
		closure_destroy_info(closure);
	}
	closure_destroy(closure);
	user_fun(ptr, context);
	if (info) {
		allocated_ancient_list_destroy((dequeue_t *)context.allocated_ancient_list);
	}
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

void scheduler_print_results() { 
	int total_space = 0;
	for (int i=0; i<NPROC; i++) {
		printf("processor %d memory usage: %dB\n", i, processors[i]->total_space);
		total_space += processors[i]->total_space;
	}
	printf("\nS1: %dB\n\n", S1);
	printf("all processors memory usage (SP): %dB\n\n", total_space);
	printf("%d(SP) <= %d(S1 * P)\n\n", total_space, S1 * NPROC);
}

void seta_start(void *fun)
{
	time_t t;
	srand((unsigned)time(&t));
	
	if (info) {
		graph_start();
	}
	
	for (int i=0; i<NPROC; i++) {
		processors[i] = processor_create(i);
		if (info) {
			processor_create_info(processors[i]);
		}
	}
	
	closure_t *closure = closure_create();
	if (info) {
		closure_create_info(closure, "entry");
		closure->allocated_ancient_list = dequeue_create();
	}
	closure_set_fun(closure, fun);
	
	processor_lock_ready_queue(processors[0]);
	ready_queue_post_closure_to_level(processors[0]->rq, closure, 0);
	processor_unlock_ready_queue(processors[0]);
	
	if (info) {
		scheduler_computate_processor_space(processors[0]);
	}
	
	for (int i=0; i<NPROC; i++) {
		processor_start(processors[i]);
	}
	for (int i=0; i<NPROC; i++) {
		processor_wait_4_me(processors[i]);
	}
	
	if (info) {
		scheduler_print_results();
	}
	
	for (int i=0; i<NPROC; i++) {
		if (info) {
			processor_destroy_info(processors[i]);
		}		
		processor_destroy(processors[i]);
	}
	
	if (info) {
		dequeue_foreach(&graph_color_S1_element, S1_list);
		allocated_ancient_list_destroy(S1_list);
		graph_finish();
	}
}
