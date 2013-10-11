/*
 *  ready_queue.c
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 10/09/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "ready_queue.h"


ready_queue_t * ready_queue_create() {
	return dequeue_create();
}

void ready_queue_destroy_level(dequeue_t *l) {
	// closures in each level should have been already destroyed
	dequeue_destroy(l);
}

void ready_queue_destroy(dequeue_t *rq) {
	dequeue_foreach(&ready_queue_destroy_level, rq);
	return dequeue_destroy(rq);
}


bool ready_queue_level_not_empty_callback(void *ptr) {
	dequeue_t *d = (dequeue_t *)ptr;
	return dequeue_is_not_empty(d);
}

closure_t * extract_head_from_deepest_level(ready_queue_t *rq) {
	dequeue_t *level = dequeue_get_last_matching_fun(&ready_queue_level_not_empty_callback, 
													 rq);
	if (level != NULL) {
		return (closure_t *)dequeue_extract_head(level);
	}
	return NULL;
}


closure_t * extract_tail_from_shallowest_level(ready_queue_t *rq) {
	dequeue_t *level = dequeue_get_first_matching_fun(&ready_queue_level_not_empty_callback,
													  rq);
	if (level != NULL) {
		return (closure_t *)dequeue_extract_tail(level);
	}
	return NULL;
}

void post_closure_to_level(ready_queue_t *rq, closure_t *cl, int level) {
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

void level_print_callback(void *level, void *acc_in, int index) {
	char *str = (char *)acc_in;
	char s1[30] = "";
	sprintf(s1, "---level %d---\n", index);
	strcat(str, s1);
	dequeue_fold(&closure_print_callback, acc_in, (dequeue_t *)level);
	strcat(str, "\n-------------\n");
}

void ready_queue_str(ready_queue_t *rq, char *str) {
	dequeue_fold(&level_print_callback, str, rq);
}

void level_space_callback(void *level, void *acc_in) {
	dequeue_fold(&closure_space_callback, acc_in, (dequeue_t *)level);
}

int ready_queue_space(ready_queue_t *rq) {
	int space = 0;
	dequeue_fold(&level_space_callback, &space, rq);
	return space;
}
