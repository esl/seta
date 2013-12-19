/*
 *  dequeue.c
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 09/09/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "dequeue.h"


dequeue_t * dequeue_create() {
    dequeue_t * d = (dequeue_t *)dmalloc(sizeof(dequeue_t));
    d->head = NULL;
    d->tail = NULL;
    d->size = 0;
    return d;
}

void dequeue_destroy(dequeue_t *l) {
    element_t *ele = l->head;
    element_t *aux = NULL;
    while(ele != NULL) {
        aux = ele;
        ele = ele->next;
        dfree(aux);
    }
    dfree(l);
}

void dequeue_foreach(void *fun, dequeue_t *d) {
    int i = 0;
    void (*callback)(void *, int) = fun;
    element_t *ele = d->head;
    while(ele != NULL) {
        callback(ele->val, i++);
        ele = ele->next;
    }
}

bool dequeue_is_not_empty(dequeue_t *d) {
    return d->head != NULL;
}
int dequeue_add_head(dequeue_t *l, void *val) {
    element_t *element = (element_t *)dmalloc(sizeof(element_t));
    element->val = val;
    element->next = l->head;
    l->head = element;
    if (l->tail == NULL) {
        l->tail = element;
    }
    l->size++;
    return 0;
}

int dequeue_add_tail(dequeue_t *l, void *val) {
    element_t *element = (element_t *)dmalloc(sizeof(element_t));
    element->val = val;
    element->next = NULL;
    if (l->head == NULL) {
        l->head = element;
        l->tail = element;
    }
    else {
        l->tail->next = element;
        l->tail = element;
    }
    l->size++;
    return 0;
}

void * dequeue_extract_head(dequeue_t *l) {
    void *res;
    element_t *aux;

    if (l->head == NULL) {
        return NULL;
    }
    else {
        res = l->head->val;
        if (l->head == l->tail) {
            dfree(l->head);
            l->head = NULL;
            l->tail = NULL;
            l->size = 0;
        }
        else {
            aux = l->head;
            l->head = l->head->next;
            dfree(aux);
            l->size--;
        }
        return res;
    }
}

void * dequeue_extract_tail(dequeue_t *l) {
    element_t *prev = l->head;
    element_t *aux = l->head;
    void *res;

    if (l->head == NULL) {
        return NULL;
    }
    else {
        res = l->tail->val;
        if (l->head == l->tail) {
            free(l->head);
            l->head = NULL;
            l->tail = NULL;
            l->size = 0;
        }
        else {
            aux = aux->next;
            while (aux->next != NULL) {
                prev = aux;
                aux = aux->next;
            }
            prev->next = NULL;
            free(aux);
            l->tail = prev;
            l->size--;
        }
        return res;
    }
}

void * dequeue_get_element(dequeue_t *l, int n) {
    int i;
    if (l->head == NULL) {
        return NULL;
    }
    else {
        element_t *aux = l->head;
        for (i=0; i<n; i++) {
            if (aux->next == NULL) {
                return NULL;
            }
            aux = aux->next;
        }
        return aux->val;
    }
}

int dequeue_size(dequeue_t *l) {
    return l->size;
}


void * dequeue_get_first_matching_fun(void *fun, dequeue_t *d) {
    bool (*callback)(void *) = fun;
    element_t *ele = d->head;
    while (ele != NULL) {
        if (callback(ele->val)) {
            return ele->val;
        }
        ele = ele->next;
    }
    return NULL;
}

void * dequeue_get_last_matching_fun(void *fun, dequeue_t *d) {
    bool (*callback)(void *) = fun;
    element_t *ele = d->head;
    void *found = NULL;
    while (ele != NULL) {
        if (callback(ele->val)) {
            found = ele->val;
        }
        ele = ele->next;
    }
    return found;
}
