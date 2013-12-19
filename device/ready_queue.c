//
//  ready_queue.c
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include "seta_internal.h"

ready_queue_t * ready_queue_create() {
    return (ready_queue_t *)dequeue_create();
}

void ready_queue_destroy_level(void *l, int g) {
    dequeue_destroy((dequeue_t *)l);
}

void ready_queue_destroy(ready_queue_t *rq) {
    dequeue_t *ready_queue = (dequeue_t *)rq;
    dequeue_foreach(&ready_queue_destroy_level, ready_queue);
    return dequeue_destroy(ready_queue);
}

bool ready_queue_is_level_not_empty(void *ptr) {
    return dequeue_is_not_empty((dequeue_t *)ptr);
}

closure_t * ready_queue_extract_head_from_deepest_level(ready_queue_t *rq) {
    dequeue_t *level = dequeue_get_last_matching_fun(&ready_queue_is_level_not_empty,
                                                     (dequeue_t *)rq);
    if (level != NULL) {
        return (closure_t *)dequeue_extract_head(level);
    }
    return NULL;
}

closure_t * ready_queue_extract_tail_from_shallowest_level(ready_queue_t *rq) {
    dequeue_t *level = dequeue_get_first_matching_fun(&ready_queue_is_level_not_empty,
                                                      rq);
    if (level != NULL) {
        return (closure_t *)dequeue_extract_tail(level);
    }
    return NULL;
}

void ready_queue_post_closure_to_level(ready_queue_t *rq, closure_t *cl, int level) {
    int i;
    dequeue_t *ready_queue = (dequeue_t *)rq;
    int missing_levels = level - dequeue_size(ready_queue) + 1;

    if (missing_levels > 0) {
        for (i=0; i<missing_levels; i++) {
            dequeue_add_tail(ready_queue, (void *)dequeue_create());
        }
    }
    dequeue_t *closures = (dequeue_t *)dequeue_get_element(ready_queue, level);
    dequeue_add_head(closures, (void *)cl);
}
