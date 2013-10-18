#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "dequeue.h"

#define NPROC 4

bool stop_computation = false;

typedef dequeue_t ready_queue_t;

ready_queue_t * ready_queue_create() {
	return dequeue_create();
}

void ready_queue_destroy_level(dequeue_t *l) {
	dequeue_destroy(l);
}

void ready_queue_destroy(dequeue_t *rq) {
	dequeue_foreach(&ready_queue_destroy_level, rq);
	return dequeue_destroy(rq);
}


typedef struct _processor {
	int id;
	ready_queue_t *rq;
	pthread_t thread;
	pthread_mutex_t rq_mutex;
} processor_t;

processor_t *processors[NPROC];

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

void processor_wait_4_me(processor_t *proc) {
	pthread_join(proc->thread, NULL);
}

void processor_lock_ready_queue(processor_t *proc) {
	pthread_mutex_lock(&proc->rq_mutex);
}

void processor_unlock_ready_queue(processor_t *proc) {
	pthread_mutex_unlock(&proc->rq_mutex);
}

typedef struct _context_t {
	int level;
	int n_local_proc;
	bool is_last_thread;
} context_t;


typedef struct _closure {
	char *name;
	int level;
	int join_counter;
	void *fun;
	void *args;
	bool is_last_thread;
	pthread_mutex_t mutex;
} closure_t;


void closure_lock(closure_t *cl) {
	pthread_mutex_lock(&cl->mutex);
}

void closure_unlock(closure_t *cl) {
	pthread_mutex_unlock(&cl->mutex);
}

typedef struct _cont {
	closure_t *cl;
	void *arg;
} cont_t;

typedef struct _handle_spawn_next {
	closure_t *cl;
} handle_spawn_next;


cont_t cont_create(void *arg, handle_spawn_next hsn) {
	cont_t cont;
	cont.cl = hsn.cl;
	cont.arg = arg;
	hsn.cl->join_counter += 1;
	return cont;
}



closure_t * closure_create() {
	closure_t *cl = (closure_t *)malloc(sizeof(closure_t));
	cl->fun = NULL;
	cl->level = 0;
	cl->join_counter = 0;
	cl->args = NULL;
	cl->is_last_thread = false;
	pthread_mutex_init(&cl->mutex, NULL);
	return cl;
}

void closure_destroy(closure_t *cl) {
	free(cl);
}

void closure_set_fun(closure_t *cl, void *fun) {
	cl->fun = fun;
}

void closure_set_args(closure_t *cl, void *args) {
	cl->args = args;
}

bool ready_queue_is_level_not_empty(void *ptr) {
	dequeue_t *d = (dequeue_t *)ptr;
	return dequeue_is_not_empty(d);
}

closure_t * ready_queue_extract_head_from_deepest_level(ready_queue_t *rq) {
	dequeue_t *level = dequeue_get_last_matching_fun(&ready_queue_is_level_not_empty, 
													 rq);
	if (level != NULL) {
		return (closure_t *)dequeue_extract_head(level);
	}
	printf("no level to extract a closure from\n");
	return NULL;
}


closure_t * ready_queue_extract_tail_from_shallowest_level(ready_queue_t *rq) {
	dequeue_t *level = dequeue_get_first_matching_fun(&ready_queue_is_level_not_empty,
													  rq);
	if (level != NULL) {
		return (closure_t *)dequeue_extract_tail(level);
	}
	printf("no level to extract a closure from\n");
	return NULL;
}

void ready_queue_post_closure_to_level(ready_queue_t *rq, closure_t *cl, int level) {
	//printf("post closure to level %d\n", level);
	int missing_levels = level - dequeue_size(*rq) + 1;
	if (missing_levels > 0) {
		//printf("...no level %d, adding missing levels before\n", level);
		for (int i=0; i<missing_levels; i++) {
			dequeue_add_tail(rq, dequeue_create());
		}
		//list *l = (list *)(rq->head->val);
		//printf("%d", ((closure *)(l->head->val))->id);
	}
	dequeue_t *closures = (dequeue_t *)dequeue_get_element(*rq, level);
	dequeue_add_head(closures, cl);
	
}

void scheduler_execute_closure(processor_t *local_proc, closure_t *cl) {
	if (cl->is_last_thread) {
		stop_computation = true;
	}
	context_t context;
	context.is_last_thread = false;
	context.n_local_proc = local_proc->id;
	context.level = cl->level;
	void (*user_fun)(void *, context_t) = cl->fun;
	void *ptr = cl->args;
	closure_destroy(cl);
	user_fun(ptr, context);
}

void *scheduling_loop(void *ptr) {
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

void processor_start(processor_t *proc) {
	pthread_create(&proc->thread, NULL, scheduling_loop, &proc->id);
}


handle_spawn_next prepare_spawn_next(void *fun, void *args, context_t *context) {
	handle_spawn_next hsn;
	closure_t *cl = closure_create();
	closure_set_fun(cl, fun);
	closure_set_args(cl, args);
	cl->level = context->level;
	cl->is_last_thread = context->is_last_thread;
	context->is_last_thread = false;
	hsn.cl = cl;
	return hsn;
}

void spawn_next(handle_spawn_next hsn) {
}

void spawn(void *fun, void *args, context_t *context) {
	closure_t *cl = closure_create();
	closure_set_fun(cl, fun);
	closure_set_args(cl, args);
	cl->level = context->level + 1;
	
	processor_t *local_proc = processors[context->n_local_proc];
	
	processor_lock_ready_queue(local_proc);
	ready_queue_post_closure_to_level(local_proc->rq, cl, cl->level);
	processor_unlock_ready_queue(local_proc);
}

void send_argument(cont_t c, void *src, int size, context_t *context) {
	closure_t *cl = c.cl;
	processor_t *local_proc = processors[context->n_local_proc];
	
	closure_lock(cl);	
	void *dst = c.arg;
	memcpy(dst, src, size);
	
	cl->join_counter -= 1;
	if (cl->join_counter == 0) {				
		processor_lock_ready_queue(local_proc);
		ready_queue_post_closure_to_level(local_proc->rq, cl, cl->level);
		processor_unlock_ready_queue(local_proc);
	}
	closure_unlock(cl);
	
}







typedef struct _args_sum {
	cont_t k;
	int x, y;
} args_sum_t;
void sum(void *ptr, context_t context) {
	args_sum_t *local_args = (args_sum_t *)ptr;
	cont_t cont = local_args->k;
	int x = local_args->x;
	int y = local_args->y;
	free(local_args);
	//---
	int res = x + y;
	send_argument(cont, &res, sizeof(res), &context);
}

typedef struct _args_fib {
	cont_t k;
	int n;
} args_fib_t;
void fib(void *ptr, context_t context) {
	args_fib_t *local_args = (args_fib_t *)ptr;
	cont_t cont = local_args->k;
	int n = local_args->n;
	free(local_args);
	//---
	if (n <= 2) { 
		int res = 1;
		send_argument(cont, &res, sizeof(res), &context);
	}
	else {
		args_sum_t *args_sum = (args_sum_t *)malloc(sizeof(args_sum_t));
		args_sum->k = cont;
		handle_spawn_next hsn = prepare_spawn_next(&sum, args_sum, &context);
		cont_t cont_l = cont_create(&args_sum->x, hsn);
		cont_t cont_r = cont_create(&args_sum->y, hsn);
		spawn_next(hsn);
		
		args_fib_t *args_fib_l = (args_fib_t *)malloc(sizeof(args_fib_t));
		args_fib_l->k = cont_l;
		args_fib_l->n = n - 2;
		spawn(&fib, args_fib_l, &context);
		
		args_fib_t *args_fib_r = (args_fib_t *)malloc(sizeof(args_fib_t));
		args_fib_r->k = cont_r;
		args_fib_r->n = n - 1;
		spawn(&fib, args_fib_r, &context);	
	}
	
}

typedef struct _args_print {
	int n;
} args_print_t;
void print(void *ptr, context_t context) {
	args_print_t *local_args = (args_print_t *)ptr;
	int res = local_args->n;
	free(local_args);
	//---
	printf("\nresult: %d\n\n", res);
}

void entry(void *ptr, context_t context) {
	context.is_last_thread = true;
	args_print_t *args_print = (args_print_t *)malloc(sizeof(args_print_t));
	handle_spawn_next hsn = prepare_spawn_next(&print, args_print, &context);
	cont_t cont = cont_create(&args_print->n, hsn);
	spawn_next(hsn);
	
	args_fib_t *args_fib = (args_fib_t *)malloc(sizeof(args_fib_t));
	args_fib->k = cont;
	args_fib->n = 35;	
	spawn(&fib, args_fib, &context);
}


void scheduler_start(void *fun)
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


int main (int argc, const char * argv[]) {
	scheduler_start(&entry);
    
	return 0;
}