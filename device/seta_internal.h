/*//
//  seta_internal.c
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//*/

#include "dequeue.h"
#include <string.h>
#include <stdarg.h>
#include "seta.h"

#define MUTEX_RQ 0
#define MUTEX_CLOSURE 1

typedef struct {
    int r;
    int c;
} rowcol_t;

typedef struct {
    int level;
    int join_counter;
    void *fun;
    void *args;
    //bool is_first_thread;
    rowcol_t core;
} closure_t;
closure_t * closure_create();
void closure_destroy(closure_t *);
void closure_destroy_info(closure_t *);
void closure_set_fun(closure_t *, void *);
void closure_set_args(closure_t *, void *);
void closure_lock(closure_t *);
void closure_unlock(closure_t *);

typedef dequeue_t ready_queue_t;
ready_queue_t * ready_queue_create();
void ready_queue_destroy(ready_queue_t *rq);
closure_t * ready_queue_extract_head_from_deepest_level(ready_queue_t *);
closure_t * ready_queue_extract_tail_from_shallowest_level(ready_queue_t *);
void ready_queue_post_closure_to_level(ready_queue_t *, closure_t *, int);

typedef struct {
    ready_queue_t *rq;
} processor_t;
processor_t * processor_create();
void processor_destroy(processor_t *);
void processor_lock_ready_queue(rowcol_t);
void processor_unlock_ready_queue(rowcol_t);

void device_write_to_host(void *, int);
void device_sync();
bool device_is_stopped();
void device_get_random_core(rowcol_t *);
//void *g_to(unsigned[], const void *);
//bool g_is_not_null(void *);
void device_mutex_lock(rowcol_t, int);
void device_mutex_unlock(rowcol_t, int);
void debug(const char *format, ...);
void *dmalloc(int);
void dfree(void *);

void scheduler_start();
