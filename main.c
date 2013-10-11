#include "seta.h"

// ----- example_base -----

typedef struct _args_spawned {
	int n1;
	cont c;
} args_spawned;
void f_spawned(void *ptr, context_t context) {
	args_spawned *args = (args_spawned *)ptr;
	int n1 = args->n1;
	send_argument(args->c, &n1, sizeof(n1), &context);
	free(args);
}

typedef struct _args_next {
	int n1, n2;
} args_next;
void f_next(void *ptr, context_t context) {
	args_next *args = (args_next *)ptr;
	printf("result: %d\n", args->n1 + args->n2);
	free(args);
}


void entry(void *ptr, context_t context) {
	args_next *args = (args_next *)malloc(sizeof(args_next));
	args->n2 = 10;
	cont c = create_cont(&args->n1);
	handle_spawn_next s = prepare_spawn_next(&f_next, args, &context);
	connect_cont(&s, &c);
	spawn_next(s);

	args_spawned *args2 = (args_spawned *)malloc(sizeof(args_spawned));
	args2->n1 = 5;
	assign_cont(&(args2->c), c);
	spawn(&f_spawned, args2, &context);
}

// ----- fibonacci -----

typedef struct _args_sum {
	cont k;
	int x, y;
} args_sum;
void sum(void *ptr, context_t context) {
	args_sum *args = (args_sum *)ptr;
	int x = args->x;
	int y = args->y;
	int z = x + y;
	
	//--- for debug
	char arg1[10] = "";
	sprintf(arg1, "%d", z);
	debug_send_argument(args->k, arg1);
	//
	
	send_argument(args->k, &z, sizeof(z), &context);
	free(args);
}

typedef struct _args_fib {
	cont k;
	int n;
} args_fib;
void fib(void *ptr, context_t context) {
	args_fib *args = (args_fib *)ptr;
	int n = args->n;
	cont c = copy_cont(args->k);
	free(args);
	
	if (n <= 2) {
		int r = 1;
		
		//--- for debug
		char arg1[10] = "";
		sprintf(arg1, "%d", r);
		debug_send_argument(c, arg1);
		//
		
		send_argument(c, &r, sizeof(r), &context);
	}
	else {		
		args_sum *args2 = (args_sum *)malloc(sizeof(args_sum));
		assign_cont(&(args2->k), c);
		cont c1 = create_cont(&args2->x);
		cont c2 = create_cont(&args2->y);
		context.spawned = "sum";
		context.args_size = sizeof(args_sum);
		//--- for debug
		debug_list_arguments_t *debug_list_arguments3 = debug_list_arguments_new();
		debug_list_arguments_add(debug_list_arguments3, "?");
		debug_list_arguments_add(debug_list_arguments3, "?");
		context.debug_list_arguments = debug_list_arguments3;
		c1.debug_n_arg = 0;
		c2.debug_n_arg = 1;
		//---		
		handle_spawn_next s = prepare_spawn_next(&sum, args2, &context);
		connect_cont(&s, &c1);
		connect_cont(&s, &c2);
		spawn_next(s);
		
		//--- for debug
		debug_list_arguments_t *debug_list_arguments = debug_list_arguments_new();
		char str1[10] = "";
		sprintf(str1, "%d", n - 2);
		debug_list_arguments_add(debug_list_arguments, str1);
		context.debug_list_arguments = debug_list_arguments;
		//---
		
		args_fib *args3 = (args_fib *)malloc(sizeof(args_fib));
		assign_cont(&(args3->k), c1);
		args3->n = n - 2;
		context.spawned = "fib_l";
		context.args_size = sizeof(args_fib);
		spawn(&fib, args3, &context);

		//--- for debug
		debug_list_arguments_t *debug_list_arguments2 = debug_list_arguments_new();
		char str2[10] = "";
		sprintf(str2, "%d", n - 1);
		debug_list_arguments_add(debug_list_arguments2, str2);
		context.debug_list_arguments = debug_list_arguments2;
		//---
		
		args_fib *args4 = (args_fib *)malloc(sizeof(args_fib));
		assign_cont(&(args4->k), c2);
		args4->n = n - 1;
		context.spawned = "fib_r";
		context.args_size = sizeof(args_fib);
		spawn(&fib, args4, &context);	
	}

	
}

typedef struct _args_print {
	int res;
} args_print;
void print(void *ptr, context_t context) {
	args_print *args = (args_print *)ptr;
	printf("\nresult: %d\n\n", args->res);
	free(args);
}


void entry2(void *ptr, context_t context) {
	args_print *args = (args_print *)malloc(sizeof(args_print));
	cont c = create_cont(&args->res);
	context.spawned = "print";
	context.args_size = sizeof(args_print);
	context.is_last_thread = true;
	
	//--- for debug
	debug_list_arguments_t *debug_list_arguments = debug_list_arguments_new();
	debug_list_arguments_add(debug_list_arguments, "?");
	context.debug_list_arguments = debug_list_arguments;
	c.debug_n_arg = 0;
	//---
	
	handle_spawn_next s = prepare_spawn_next(&print, args, &context);
	connect_cont(&s, &c);
	spawn_next(s);
	
	args_fib *args2 = (args_fib *)malloc(sizeof(args_fib));
	args2->n = 7;
	assign_cont(&(args2->k), c);
	context.spawned = "fib";
	context.args_size = sizeof(args_fib);
	
	//--- for debug
	debug_list_arguments_t *debug_list_arguments2 = debug_list_arguments_new();
	//char str1[10] = "";
	//sprintf(str1, "%s", args2->n);
	debug_list_arguments_add(debug_list_arguments2, "7");
	context.debug_list_arguments = debug_list_arguments2;
	//---
	
	spawn(&fib, args2, &context);
}



int main(void) {
	int closure_size = sizeof(closure_t) + NAMELEN;
	int print_closure_size = closure_size + sizeof(args_print);
	int fib_closure_size = closure_size + sizeof(args_fib);
	int sum_closure_size = closure_size + sizeof(args_sum);
	
	printf("------closure size------\n");
	printf("start closure: %dB\n", closure_size);
	printf("print closure: %dB\n", print_closure_size);
	printf("fib closure: %dB\n", fib_closure_size);
	printf("sum closure: %dB\n", sum_closure_size);
	printf("------------------------\n\n");
	
	seta_start(&entry2);
	
	return 0;
}