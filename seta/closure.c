#include "seta_internal.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

closure_t * closure_create() {
	closure_t *closure = (closure_t *)malloc(sizeof(closure_t));
	closure->fun = NULL;
	closure->level = 0;
	closure->join_counter = 0;
	closure->args = NULL;
	closure->is_last_thread = false;
	pthread_mutex_init(&closure->mutex, NULL);
	return closure;
}

closure_t * closure_create_info(closure_t *closure, char *fun_name) {
	closure->args_size = 0;
	closure->allocated_ancients = 0;
	closure->arg_name_list = NULL;
	char *name = (char *)malloc(sizeof(char) * (strlen(fun_name) + 1));
	strcpy(name, fun_name);
	closure->name = name;
	return closure;
}

void closure_destroy_arg_name(char *arg) {
	free(arg);
}

void closure_destroy_info(closure_t *closure) {
	free(closure->name);
	if (closure->arg_name_list != NULL) {
		dequeue_foreach(&closure_destroy_arg_name, closure->arg_name_list);
		dequeue_destroy(closure->arg_name_list);
	}
}

void closure_destroy(closure_t *closure) {
	pthread_mutex_destroy(&closure->mutex);
	free(closure);
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

int closure_space(closure_t *closure) {
	return sizeof(closure_t) + sizeof(char) * (strlen(closure->name) + 1) + 
		closure->args_size;
}

void closure_space_cb(void *ptr, void *acc_in, int index) {
	*(int *)acc_in += closure_space((closure_t *)ptr);
}