/*
 *  further_use.c
 *  seta
 *
 *  Created by Fabio Pricoco on 28/10/2013.
 *  Copyright 2013 Erlang Solution Ltd. All rights reserved.
 *
 */


#include "seta.h"
#include <stdio.h>
#include <stdlib.h>

bool graph = true;

typedef struct _args_print {
	int val;
} args_print_t;
void print(seta_context_t context) {
	args_print_t *local_args = (args_print_t *)seta_get_args(&context);
	int val = local_args->val;
	seta_free_args(&context);
	//---
	printf("result: %d\n", val);
}

typedef struct _args_continuing {
	int init_n;
	int n;
} args_continuing_t;
void continuing(seta_context_t context) {
	args_continuing_t *local_args = (args_continuing_t *)seta_get_args(&context);
	int init_n = local_args->init_n;
	int n = local_args->n;
	seta_free_args(&context);
	//---
	n--;
	if (n > 0) {
		//------ spawn_next_continuing ------
		if (graph) {
			context.spawned = "continuing";
			seta_arg_name_list_t *arg_name_list = seta_arg_name_list_new();
			seta_arg_name_list_add(arg_name_list, "%d", init_n);
			seta_arg_name_list_add(arg_name_list, "%d", n);
			context.arg_name_list = arg_name_list;
		}		
		args_continuing_t *args_continuing = (args_continuing_t *)seta_alloc_args(sizeof(args_continuing_t));
		args_continuing->init_n = init_n;
		args_continuing->n = n;
		seta_handle_spawn_next_t hsn = seta_prepare_spawn_next(&continuing, args_continuing, &context);
		seta_spawn_next(hsn);	
	}
	else {
		if (graph) {
			context.spawned = "print";
			seta_arg_name_list_t *arg_name_list = seta_arg_name_list_new();
			seta_arg_name_list_add(arg_name_list, "%d", init_n);
			context.arg_name_list = arg_name_list;
		}	
		//------ spawn_next_print ------
		context.is_last_thread = true;
		args_print_t *args_print = (args_print_t *)seta_alloc_args(sizeof(args_print_t));
		args_print->val = init_n;
		seta_handle_spawn_next_t hsn = seta_prepare_spawn_next(&print, args_print, &context);
		seta_spawn_next(hsn);
	}
}

void entry(seta_context_t context) {
	//------ spawn_print1 ------
	if (graph) {
		context.spawned = "print";
		seta_arg_name_list_t *arg_name_list = seta_arg_name_list_new();
		seta_arg_name_list_add(arg_name_list, "%d", 1);
		context.arg_name_list = arg_name_list;
	}
	args_print_t *args_print1 = (args_print_t *)seta_alloc_args(sizeof(args_print_t));
	args_print1->val = 1;
	seta_spawn(&print, args_print1, &context);

	//------ spawn_print2 ------
	if (graph) {
		context.spawned = "print";
		seta_arg_name_list_t *arg_name_list = seta_arg_name_list_new();
		seta_arg_name_list_add(arg_name_list, "%d", 2);
		context.arg_name_list = arg_name_list;
	}
	args_print_t *args_print2 = (args_print_t *)seta_alloc_args(sizeof(args_print_t));
	args_print2->val = 2;
	seta_spawn(&print, args_print2, &context);

	//------ spawn_print3 ------
	if (graph) {
		context.spawned = "print";
		seta_arg_name_list_t *arg_name_list = seta_arg_name_list_new();
		seta_arg_name_list_add(arg_name_list, "%d", 3);
		context.arg_name_list = arg_name_list;
	}
	args_print_t *args_print3 = (args_print_t *)seta_alloc_args(sizeof(args_print_t));
	args_print3->val = 3;
	seta_spawn(&print, args_print3, &context);

	//------ spawn_print4 ------
	if (graph) {
		context.spawned = "print";
		seta_arg_name_list_t *arg_name_list = seta_arg_name_list_new();
		seta_arg_name_list_add(arg_name_list, "%d", 4);
		context.arg_name_list = arg_name_list;
	}
	args_print_t *args_print4 = (args_print_t *)seta_alloc_args(sizeof(args_print_t));
	args_print4->val = 4;
	seta_spawn(&print, args_print4, &context);

	//------ spawn_print5 ------
	if (graph) {
		context.spawned = "print";
		seta_arg_name_list_t *arg_name_list = seta_arg_name_list_new();
		seta_arg_name_list_add(arg_name_list, "%d", 5);
		context.arg_name_list = arg_name_list;
	}
	args_print_t *args_print5 = (args_print_t *)seta_alloc_args(sizeof(args_print_t));
	args_print5->val = 5;
	
	seta_spawn(&print, args_print5, &context);
	
	//------ spawn_next_continuing ------
	if (graph) {
		context.spawned = "continuing";
		seta_arg_name_list_t *arg_name_list = seta_arg_name_list_new();
		seta_arg_name_list_add(arg_name_list, "100");
		seta_arg_name_list_add(arg_name_list, "100");
		context.arg_name_list = arg_name_list;
	}
	args_continuing_t *args_continuing = (args_continuing_t *)seta_alloc_args(sizeof(args_continuing_t));
	args_continuing->init_n = 100;
	args_continuing->n = 100;
	seta_handle_spawn_next_t hsn = seta_prepare_spawn_next(&continuing, args_continuing, &context);
	seta_spawn_next(hsn);
}




int main (int argc, const char * argv[]) {
	if (graph) {
		seta_enable_graph("further_use.dot");
	}
	return seta_start(&entry, 2);
}
