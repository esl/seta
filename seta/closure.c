#include "seta_internal.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

int closure_id = 0;

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
	closure->allocated_ancient_list = NULL;
	closure->arg_name_list = NULL;
	msg_t name = msg_new_from(fun_name);
	closure->name = name;
	closure->id = closure_id++;
	return closure;
}

void closure_arg_name_destroy(char **arg_ptr) {
	free(*arg_ptr);
	free(arg_ptr);
}

void closure_destroy_info(closure_t *closure) {
	free(closure->name);
	if (closure->arg_name_list != NULL) {
		dequeue_foreach(&closure_arg_name_destroy, closure->arg_name_list);
		dequeue_destroy(closure->arg_name_list);
	}
	allocated_ancient_list_destroy(closure->allocated_ancient_list);
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
	return sizeof(closure_t) + sizeof(char) * ((int)strlen(closure->name) + 1) +
		closure->args_size;
}

void closure_space_cb(void *ptr, void *acc_in, int index) {
	*(int *)acc_in += closure_space((closure_t *)ptr);
}

void list_arguments_str(void *ptr, void **acc, int index) {
	char **str = (char **)ptr;
	char **res = (char **)acc;
	msg_cat(res, *str);
	msg_cat(res, ",");
}

void closure_str(char **acc, closure_t *closure) {
	msg_cat(acc, closure->name);
	
	if (closure->arg_name_list != NULL) {
		msg_t args_str = msg_new_from("(");
		dequeue_fold(&list_arguments_str, &args_str, closure->arg_name_list);
		msg_ncat(acc, args_str, (int)strlen(args_str) - 1);
		msg_destroy(args_str);
		msg_cat(acc, ")");
	}
}
