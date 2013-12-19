//
//  seta.h
//  seta
//
//  Created by Fabio Pricoco on 21/11/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    void *closure;
    void *arg;
} seta_cont_t;

typedef struct {
    void *args;
    int level;
    //bool is_first_thread;
    //bool free_args_done;
    //bool spawn_next_done;

} seta_context_t;

typedef struct {
    void *closure;
} seta_handle_spawn_next_t;


seta_cont_t seta_create_cont(void *, seta_handle_spawn_next_t);

void * seta_alloc_args(long);

void seta_free_args(seta_context_t *);

seta_handle_spawn_next_t seta_prepare_spawn_next(void *, void *, seta_context_t *);

void seta_spawn_next(seta_handle_spawn_next_t);

void seta_spawn(void *fun, void *, seta_context_t *);

void seta_send_argument(seta_cont_t *, void *, int, seta_context_t *);

void seta_send_argument();
