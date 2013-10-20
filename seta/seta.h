//
//  seta.h
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include <stdbool.h>

typedef struct _handle_spawn_next {
	void *closure;
} handle_spawn_next_t;

typedef struct _seta_cont {
	void *closure;
	void *arg;
} seta_cont_t;

seta_cont_t seta_cont_create(void *, handle_spawn_next_t);


typedef struct _seta_context_t {
	int level;
	int n_local_proc;
	bool is_last_thread;
} seta_context_t;



void seta_start(void *);

handle_spawn_next_t seta_prepare_spawn_next(void *, void *, seta_context_t *);

void seta_spawn_next(handle_spawn_next_t);

void seta_spawn(void *fun, void *, seta_context_t *);

void seta_send_argument(seta_cont_t, void *, int, seta_context_t *);


