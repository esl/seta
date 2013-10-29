/*
 *  lists.c
 *  seta
 *
 *  Created by Fabio Pricoco on 24/10/2013.
 *  Copyright 2013 Erlang Solution Ltd. All rights reserved.
 *
 */

#include "seta_internal.h"

msg_list_t * msg_list_create() {
	return (msg_list_t *)dequeue_create();
}

void msg_list_append(msg_list_t *msg_list, char *str) {
	dequeue_add_tail((dequeue_t *)msg_list, msg_new_from_str(str));
}

void msg_list_append_reverse(msg_list_t *msg_list, char *str) {
	dequeue_add_head((dequeue_t *)msg_list, msg_new_from_str(str));
}

void msg_list_append_int(msg_list_t *msg_list, int i) {
	dequeue_add_tail((dequeue_t *)msg_list, msg_new_from_int(i));
}

void msg_list_append_int_reverse(msg_list_t *msg_list, int i) {
	dequeue_add_head((dequeue_t *)msg_list, msg_new_from_int(i));
}

void msg_list_set(msg_list_t *msg_list, int index, char *str) {
	dequeue_t *dequeue = (dequeue_t *)msg_list;
	msg_destroy((msg_t)dequeue_get_element(*dequeue, index));
	dequeue_set_element(dequeue, index, (void *)msg_new_from_str(str));
}

void msg_str(void *val_ptr, void *acc_ptr, int index) {
	char *str = (char *)val_ptr;
	msg_t *acc = (msg_t *)acc_ptr;
	msg_cat(acc, str);
	msg_cat(acc, ",");
}

void msg_list_str(msg_t *acc, msg_list_t *msg_list) {
	dequeue_t *dequeue = (dequeue_t *)msg_list;
	dequeue_fold(&msg_str, acc, dequeue);
}

void * msg_copy(void *msg) {
	return (void *)msg_new_from_str((char *)msg);
}

msg_list_t * msg_list_copy(msg_list_t *msg_list) {
	return (msg_list_t *)dequeue_copy(&msg_copy, (dequeue_t *)msg_list);
}

void msg_list_foreach(void *fun, msg_list_t *msg_list) {
	dequeue_foreach(fun, (dequeue_t *)msg_list);
}

void msg_list_destroy(msg_list_t *msg_list) {
	dequeue_t *dequeue = (dequeue_t *)msg_list;
	if (dequeue != NULL) {
		dequeue_foreach(&msg_destroy, dequeue);
		dequeue_destroy(dequeue);
	}
}

void msg_print(void *msg) {
	printf("%s\n", (char *)msg);
}

void msg_list_print(msg_list_t *msg_list) {
	dequeue_foreach(&msg_print, (dequeue_t *)msg_list);
}






spawn_list_t * spawn_list_create() {
	return (msg_list_t *)dequeue_create();
}

void spawn_element_destroy(void *ptr) {
	spawn_element_t *spawn_element = (spawn_element_t *)ptr;
	free(spawn_element->label);
	free(spawn_element);
}

void spawn_list_destroy(spawn_list_t *spawn_list) {
	dequeue_t *dequeue = (dequeue_t *)spawn_list;
	dequeue_foreach(&spawn_element_destroy, dequeue);
	dequeue_destroy(dequeue);
}


void spawn_list_append(spawn_list_t *spawn_list, int id, char *label) {
	spawn_element_t *spawn_element = (spawn_element_t *)malloc(sizeof(spawn_element_t));
	spawn_element->id = id;
	spawn_element->label = msg_new_from_str(label);
	dequeue_add_head((dequeue_t *)spawn_list, (void *)spawn_element);
}

