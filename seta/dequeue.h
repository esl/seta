/*
 *  linked_list.h
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 09/09/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _element
{
    void *val;
    struct _element *next;
} element_t;

typedef struct _dequeue {
	element_t *head;
	element_t *tail;
	int size;
} dequeue_t;

typedef struct _dequeue_iterator {
	element_t *cur_element;
} dequeue_iterator_t;


dequeue_t * dequeue_create();

dequeue_t * dequeue_copy(void *, dequeue_t *);

void dequeue_destroy(dequeue_t *);

int dequeue_add_head(dequeue_t *, void *);

int dequeue_add_tail(dequeue_t *, void *);

void * dequeue_extract_head(dequeue_t *);

void * dequeue_extract_tail(dequeue_t *);

int dequeue_remove_element_by_value(dequeue_t *, void *);

void * dequeue_get_element(dequeue_t, int);

int dequeue_set_element(dequeue_t *d, int n, void *new_val);

void * dequeue_get_last(dequeue_t);

int dequeue_size(dequeue_t);

void dequeue_fold(void *, void *, dequeue_t *);

void dequeue_foreach(void *, dequeue_t *);

bool dequeue_is_not_empty(dequeue_t *);

void * dequeue_get_first_matching_fun(void *, dequeue_t *);

void * dequeue_get_last_matching_fun(void *, dequeue_t *);


dequeue_iterator_t dequeue_get_iterator(dequeue_t *);

bool dequeue_iterator_has_more(dequeue_iterator_t *);

void * dequeue_iterator_get_next(dequeue_iterator_t *);


