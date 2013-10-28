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
#include <string.h>
#include <stdarg.h>

typedef struct _spawn_element {
	int id;
	char *label;
} spawn_element_t;

typedef dequeue_t spawn_list_t;
spawn_list_t * spawn_list_create();
void spawn_list_append(spawn_list_t *, int, char *);
void spawn_list_destroy(spawn_list_t *);


typedef char *msg_t;
msg_t msg_new();
msg_t msg_new_from_str(char *);
msg_t msg_new_from_int(int);
void msg_destroy(msg_t);
void msg_cat(msg_t *, const char *, ...);
void msg_ncat(msg_t *, char *, int);


typedef dequeue_t msg_list_t;
msg_list_t * msg_list_create();
void msg_list_append(msg_list_t *, char *);
void msg_list_append_reverse(msg_list_t *, char *);
void msg_list_append_int(msg_list_t *, int);
void msg_list_append_int_reverse(msg_list_t *, int);
void msg_list_set(msg_list_t *, int, char *);
void msg_list_str(msg_t *, msg_list_t *);
msg_list_t * msg_list_copy(msg_list_t *);
void msg_list_foreach(void *, msg_list_t *);
void msg_list_destroy(msg_list_t *);
void msg_list_print(msg_list_t *);


typedef struct _closure {
	int level;
	int join_counter;
	void *fun;
	void *args;
	bool is_last_thread;
	pthread_mutex_t mutex;
	int proc;
	// info
	char *name;
	int args_size;
	msg_list_t *arg_name_list;
	int allocated_ancients;
	msg_list_t *allocated_ancient_list;
	int id;
} closure_t;
closure_t * closure_create();
closure_t * closure_create_info(closure_t *);
closure_t * closure_create_graph(closure_t *, char *);
void closure_destroy_graph(closure_t *);
void closure_destroy_info(closure_t *);
void closure_destroy(closure_t *);
void closure_set_fun(closure_t *, void *);
void closure_set_args(closure_t *, void *);
void closure_lock(closure_t *);
void closure_unlock(closure_t *);
int closure_space(closure_t *);
void closure_space_cb(void *, void *, int);
void closure_str(char **, closure_t *);

typedef dequeue_t ready_queue_t;

typedef struct _processor {
	int id;
	ready_queue_t *rq;
	pthread_t thread;
	pthread_mutex_t rq_mutex;
	// info
	dequeue_t *stalled;
	pthread_mutex_t stalled_mutex;
	int cur_space;
	int total_space;
} processor_t;
processor_t * processor_create(int);
void processor_create_info(processor_t *);
void processor_destroy_info(processor_t *);
void processor_destroy(processor_t *);
void processor_start(processor_t *);
void processor_wait_4_me(processor_t *);
void processor_lock_ready_queue(processor_t *);
void processor_unlock_ready_queue(processor_t *);
void processor_lock_stalled(processor_t *);
void processor_unlock_stalled(processor_t *);
void processor_add_to_stalled(processor_t *, closure_t *);
void processor_remove_from_stalled(processor_t *, closure_t *);
void processor_computate_space(processor_t *);

ready_queue_t * ready_queue_create();
void ready_queue_destroy(dequeue_t *rq);
closure_t * ready_queue_extract_head_from_deepest_level(ready_queue_t *);
closure_t * ready_queue_extract_tail_from_shallowest_level(ready_queue_t *);
void ready_queue_post_closure_to_level(ready_queue_t *, closure_t *, int);
int ready_queue_space(ready_queue_t *);

void *scheduler_scheduling_loop(void *);


typedef struct _logger {
	FILE *fp;
} logger_t;
logger_t logger_create(char *);
void logger_destroy(logger_t);
void logger_write(logger_t logger, const char *format, ...);


void graph_start(char *);
void graph_finish();
void graph_set_color(int, int);
void graph_spawn_next(int, int);
void graph_spawns(int, spawn_list_t *);
void graph_send_argument(closure_t *, seta_context_t *);
void graph_enable(int, char *);
void graph_color_S1_element(void *, int);


