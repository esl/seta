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
	logger = logger_create("spawn_tree.dot");
	msg_t msg = msg_new();
	msg_cat(&msg, "digraph G {\n"
			"\tnode [style=filled, fillcolor=white];\n"
			"\t\"cl0\"[label=entry];\n");
	logger_write(logger, msg);
	msg_destroy(msg);
}

void graph_execute(closure_t *closure, int n_local_proc) {
    logger_write(logger, "\t\"cl%d\"[color=%s];\n", closure->id, color[n_local_proc]);
}

void graph_spawn_next(closure_t *closure, seta_context_t *context) {
	logger_write(logger, "\t{rank=same; \"cl%d\" -> \"cl%d\"};\n", 
				 context->closure_id, closure->id);
}

void graph_spawn(closure_t *closure, seta_context_t *context) {
	msg_t msg = msg_new();
	msg_cat(&msg, "\t\"cl%d\" -> \"cl%d\";\n", context->closure_id, closure->id);
	msg_cat(&msg, "\t\"cl%d\"[label=\"", closure->id);
	closure_str(&msg, closure);
	msg_cat(&msg, "\"];\n");
	logger_write(logger, msg);
	msg_destroy(msg);
}

void write_node(void *cl, msg_t *acc, int g) {
	msg_cat(acc, "\"cl%s\";", (char *)cl);
}

void graph_rearrange_spawns(msg_list_t *spawn_id_list) {
	msg_t msg = msg_new();
	msg_cat(&msg, "\t{rank=same; ");
	dequeue_fold(&write_node, &msg, spawn_id_list);
	//msg_list_foreach(&write_node, spawn_id_list);
	msg_cat(&msg, "}\n");
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

void graph_color_S1_element(void *cl, int g) {
	logger_write(logger, "\t\"cl%s\"[fillcolor=yellow];\n", (char *)cl);
}

void graph_finish() {
	logger_write(logger, "}");
	logger_destroy(logger);
}
