/*
 *  graph_dot.c
 *  seta
 *
 *  Created by Fabio Pricoco on 22/10/2013.
 *  Copyright 2013 Erlang Solution Ltd. All rights reserved.
 *
 */

#include "seta_internal.h"

logger_t logger;

char *color[] = {"green", "red", "blue", "orange", "gray"};

void graph_start() {
	logger = logger_create("/Users/fabio/Desktop/seta/spawn_tree.dot");
	msg_t msg = msg_new();
	msg_cat(&msg, "digraph G {\n"
			"\tnode [color=green];\n"
			"\t\"cl0\"[label=entry];\n");
	logger_write(logger, msg);
	msg_destroy(msg);	
}

void graph_spawn_next(closure_t *closure, seta_context_t *context) {
	msg_t msg = msg_new();
    msg_cat(&msg, "\t\"cl%d\"[color=%s];\n", closure->id, color[context->n_local_proc]);
	msg_cat(&msg, "\t{rank=same; \"cl%d\" -> \"cl%d\"};\n", context->closure_id, closure->id);
	logger_write(logger, msg);
	msg_destroy(msg);
}

void graph_spawn(closure_t *closure, seta_context_t *context) {
	msg_t msg = msg_new();
    msg_cat(&msg, "\t\"cl%d\"[color=%s];\n", closure->id, color[context->n_local_proc]);
	msg_cat(&msg, "\t\"cl%d\" -> \"cl%d\";\n", context->closure_id, closure->id);
	msg_cat(&msg, "\t\"cl%d\"[label=\"", closure->id);
	closure_str(&msg, closure);
	msg_cat(&msg, "\"];\n");
	logger_write(logger, msg);
	msg_destroy(msg);
}

void graph_send_argument(closure_t *closure, seta_context_t *context) {
	logger_write(logger, "\t\"cl%d\" -> \"cl%d\"[constraint=false, style=dashed];\n",
                 context->closure_id, closure->id);
}

void graph_send_argument_enable(closure_t *closure, seta_context_t *context) {
	msg_t msg = msg_new();
	msg_cat(&msg, "\t\"cl%d\"[label=\"", closure->id);
	closure_str(&msg, closure);
	msg_cat(&msg, "\"];\n");
	logger_write(logger, msg);
	msg_destroy(msg);
}

void graph_finish() {
	logger_write(logger, "}");
	logger_destroy(logger);
}
