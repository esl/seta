//
//  seta_program.c
//  seta_parallella
//
//  Created by Fabio Pricoco on 21/11/13.
//
//

#include "seta.h"

void debug(const char *format, ...);


typedef struct _args_sum {
	seta_cont_t k;
	int x, y;
} args_sum_t;
void sum(seta_context_t context) {
	args_sum_t *local_args = (args_sum_t *)seta_get_args(&context);
	seta_cont_t cont = local_args->k;
	int x = local_args->x;
	int y = local_args->y;
	seta_free_args(&context);

	//------ send_argument ------
	int res = x + y;
	seta_send_argument(&cont, &res, sizeof(res), &context);
}

typedef struct _args_fib {
	seta_cont_t k;
	int n;
} args_fib_t;
void fib(seta_context_t context) {
	args_fib_t *local_args = (args_fib_t *)seta_get_args(&context);
	seta_cont_t cont = local_args->k;
	int n = local_args->n;
	seta_free_args(&context);
	//---
	if (n <= 2) {
		//------ send_argument ------
		int res = 1;
		seta_send_argument(&cont, &res, sizeof(res), &context);
	}
	else {
		//------ spawn_next_sum ------
        args_sum_t *args_sum = (args_sum_t *)seta_alloc_args(sizeof(args_sum_t));
		args_sum->k = cont;
		seta_handle_spawn_next_t hsn = seta_prepare_spawn_next(&sum, args_sum, &context);
		seta_cont_t cont_l = seta_create_cont(&args_sum->x, hsn);
		seta_cont_t cont_r = seta_create_cont(&args_sum->y, hsn);
		seta_spawn_next(hsn);

		//------ spawn_fib ------
		args_fib_t *args_fib_r = (args_fib_t *)seta_alloc_args(sizeof(args_fib_t));
		args_fib_r->k = cont_r;
		args_fib_r->n = n - 1;
		seta_spawn(&fib, args_fib_r, &context);

		//------ spawn_fib ------
		args_fib_t *args_fib_l = (args_fib_t *)seta_alloc_args(sizeof(args_fib_t));
		args_fib_l->k = cont_l;
		args_fib_l->n = n - 2;
		seta_spawn(&fib, args_fib_l, &context);
	}
}

typedef struct _args_print {
	int n;
} args_print_t;
void print(seta_context_t context) {
	args_print_t *local_args = (args_print_t *)seta_get_args(&context);
	int res = local_args->n;
	seta_free_args(&context);
	//---
	seta_send_argument(NULL, &res, 4, &context);
}

void entry(seta_context_t context) {
    int *in = (int *)seta_get_args(&context);
    //device_stop_and_send_result(in, 4);
	//seta_free_args(&context);
	//---
	//------ spawn_next_print ------
	args_print_t *args_print = (args_print_t *)seta_alloc_args(sizeof(args_print_t));
	seta_handle_spawn_next_t hsn = seta_prepare_spawn_next(&print, args_print, &context);
	seta_cont_t cont = seta_create_cont(&args_print->n, hsn);
	seta_spawn_next(hsn);

	//------ spawn_fib ------
	args_fib_t *args_fib = (args_fib_t *)seta_alloc_args(sizeof(args_fib_t));
	args_fib->k = cont;
	args_fib->n = *in;
	seta_spawn(&fib, args_fib, &context);
}


/*typedef struct __attribute__((__packed__)){
	seta_cont_t k;
	int n;
} args_child_t;
void child(seta_context_t context) {
	args_child_t *local_args = (args_child_t *)seta_get_args(&context);
	seta_cont_t cont = local_args->k;
	int n = local_args->n;
	seta_free_args(&context);
	//---
	seta_send_argument(&cont, &n, sizeof(n), &context);
}

typedef struct __attribute__((__packed__)){
	int n;
} args_print_t;
void print(seta_context_t context) {
	args_print_t *local_args = (args_print_t *)seta_get_args(&context);
	int res = local_args->n;
	seta_free_args(&context);
	//---
	seta_send_argument(NULL, &res, 4, &context);
	//printf("\nresult: %d\n\n", res);
}

void entry(seta_context_t context) {
	//------ spawn_next_print ------
	//debug("0x%x", seta_alloc_args(10));
	args_print_t *args_print = (args_print_t *)seta_alloc_args(sizeof(args_print_t));
	seta_handle_spawn_next_t hsn = seta_prepare_spawn_next(&print, args_print, &context);
	seta_cont_t cont = seta_create_cont(&args_print->n, hsn);
	seta_spawn_next(hsn);

	//------ spawn_child ------
	args_child_t *args_child = (args_child_t *)seta_alloc_args(sizeof(args_child_t));
	args_child->k = cont;
	args_child->n = 10;
	seta_spawn(&child, args_child, &context);
}*/

