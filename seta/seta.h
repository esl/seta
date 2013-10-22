//
//  seta.h
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include <stdbool.h>

typedef struct _seta_handle_spawn_next {
	void *closure;
} seta_handle_spawn_next_t;

typedef struct _seta_cont {
	void *closure;
	void *arg;
	// info
	int n_arg;
} seta_cont_t;

seta_cont_t seta_cont_create(void *, seta_handle_spawn_next_t);


typedef struct _seta_context_t {
	int level;
	int n_local_proc;
	bool is_last_thread;
	// info
	char *spawned;
	int args_size;
	void *arg_name_list;
	int allocated_ancients;
} seta_context_t;

typedef void *seta_arg_name_list_t;


void seta_start(void *);

seta_handle_spawn_next_t seta_prepare_spawn_next(void *, void *, seta_context_t *);

void seta_spawn_next(seta_handle_spawn_next_t);

void seta_spawn(void *fun, void *, seta_context_t *);

void seta_send_argument(seta_cont_t, void *, int, seta_context_t *);

void seta_enable_info();

seta_arg_name_list_t seta_arg_name_list_new();

void seta_arg_name_list_add(seta_arg_name_list_t, char *);

void seta_send_arg_name(seta_cont_t, char *);
