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

closure_t * ready_queue_extract_head_from_deepest_level(ready_queue_t *);

closure_t * ready_queue_extract_tail_from_shallowest_level(ready_queue_t *);

void ready_queue_post_closure_to_level(ready_queue_t *, closure_t *, int);

int ready_queue_space(ready_queue_t *);

void ready_queue_str(char **, ready_queue_t *);
