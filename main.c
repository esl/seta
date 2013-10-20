#include "seta.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct _args_sum {
	seta_cont_t k;
	int x, y;
} args_sum_t;
void sum(void *ptr, seta_context_t context) {
	args_sum_t *local_args = (args_sum_t *)ptr;
	seta_cont_t cont = local_args->k;
	int x = local_args->x;
	int y = local_args->y;
	free(local_args);
	//---
	int res = x + y;
	seta_send_argument(cont, &res, sizeof(res), &context);
}

typedef struct _args_fib {
	seta_cont_t k;
	int n;
} args_fib_t;
void fib(void *ptr, seta_context_t context) {
	args_fib_t *local_args = (args_fib_t *)ptr;
	seta_cont_t cont = local_args->k;
	int n = local_args->n;
	free(local_args);
	//---
	if (n <= 2) { 
		int res = 1;
		seta_send_argument(cont, &res, sizeof(res), &context);
	}
	else {
		args_sum_t *args_sum = (args_sum_t *)malloc(sizeof(args_sum_t));
		args_sum->k = cont;
		handle_spawn_next_t hsn = seta_prepare_spawn_next(&sum, args_sum, &context);
		seta_cont_t cont_l = seta_cont_create(&args_sum->x, hsn);
		seta_cont_t cont_r = seta_cont_create(&args_sum->y, hsn);
		seta_spawn_next(hsn);
		
		args_fib_t *args_fib_l = (args_fib_t *)malloc(sizeof(args_fib_t));
		args_fib_l->k = cont_l;
		args_fib_l->n = n - 2;
		seta_spawn(&fib, args_fib_l, &context);
		
		args_fib_t *args_fib_r = (args_fib_t *)malloc(sizeof(args_fib_t));
		args_fib_r->k = cont_r;
		args_fib_r->n = n - 1;
		seta_spawn(&fib, args_fib_r, &context);
	}
	
}

typedef struct _args_print {
	int n;
} args_print_t;
void print(void *ptr, seta_context_t context) {
	args_print_t *local_args = (args_print_t *)ptr;
	int res = local_args->n;
	free(local_args);
	//---
	printf("\nresult: %d\n\n", res);
}

void entry(void *ptr, seta_context_t context) {
	context.is_last_thread = true;
	args_print_t *args_print = (args_print_t *)malloc(sizeof(args_print_t));
	handle_spawn_next_t hsn = seta_prepare_spawn_next(&print, args_print, &context);
	seta_cont_t cont = seta_cont_create(&args_print->n, hsn);
	seta_spawn_next(hsn);
	
	args_fib_t *args_fib = (args_fib_t *)malloc(sizeof(args_fib_t));
	args_fib->k = cont;
	args_fib->n = 35;	
	seta_spawn(&fib, args_fib, &context);
}




int main (int argc, const char * argv[]) {
	seta_start(&entry);
    
	return 0;
}