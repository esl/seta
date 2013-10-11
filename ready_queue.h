/*
 *  ready_queue.h
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 10/09/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "closure.h"

#include <string.h>

typedef dequeue_t ready_queue_t;


ready_queue_t * ready_queue_create();

void ready_queue_destroy(ready_queue_t *);

closure_t * extract_head_from_deepest_level(ready_queue_t *);

closure_t * extract_tail_from_shallowest_level(ready_queue_t *);

void post_closure_to_level(ready_queue_t *, closure_t *, int);

int ready_queue_space(ready_queue_t *);

//void ready_queue_level_print_callback(void *, int);

//void ready_queue_print(ready_queue *);

void ready_queue_str(ready_queue_t *, char *);