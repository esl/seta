#include "seta_internal.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

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

void closure_lock(closure_t *cl) {
	pthread_mutex_lock(&cl->mutex);
}

void closure_unlock(closure_t *cl) {
	pthread_mutex_unlock(&cl->mutex);
}