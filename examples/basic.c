/*
 *  basic.c
 *  seta
 *
 *  Created by Fabio Pricoco on 28/10/2013.
 *  Copyright 2013 Erlang Solution Ltd. All rights reserved.
 *
 */


#include "seta.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct _args_child {
	seta_cont_t k;
	int n;
} args_child_t;
void child(seta_context_t context) {
	args_child_t *local_args = (args_child_t *)seta_get_args(&context);
	seta_cont_t cont = local_args->k;
	int n = local_args->n;
	seta_free_args(&context);
	//---
	seta_send_argument(cont, &n, sizeof(n), &context);
}

typedef struct _args_print {
	int n;
} args_print_t;
void print(seta_context_t context) {
	args_print_t *local_args = (args_print_t *)seta_get_args(&context);
	int res = local_args->n;
	seta_free_args(&context);
	//---
	printf("\nresult: %d\n\n", res);
}

void entry(seta_context_t context) {
	//------ spawn_next_print ------
	args_print_t *args_print = (args_print_t *)seta_alloc_args(sizeof(args_print_t));
	seta_last_thread(&context);
	seta_handle_spawn_next_t hsn = seta_prepare_spawn_next(&print, args_print, &context);
	seta_cont_t cont = seta_create_cont(&args_print->n, hsn);
	seta_spawn_next(hsn);
	
	//------ spawn_child ------
	args_child_t *args_child = (args_child_t *)seta_alloc_args(sizeof(args_child_t));
	args_child->k = cont;
	args_child->n = 10;
	seta_spawn(&child, args_child, &context);
}




int main (int argc, const char * argv[]) {
	if (argc == 2) {
		return seta_start(&entry, atoi(argv[1]));
	}
	else {
		printf("execute: ./basic n_threads\n");
		return -1;
	}
}