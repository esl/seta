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
			"\tnode [style=filled, fillcolor=white];\n"
			"\t\"cl0\"[label=entry];\n");
	logger_write(logger, msg);
	msg_destroy(msg);
}

void graph_set_color(int id, int n_local_proc) {
    logger_write(logger, "\t\"cl%d\"[color=%s];\n", id, color[n_local_proc]);
}

void graph_spawn_next(int spawning_id, int spawned_id) {
	logger_write(logger, "\t{rank=same; \"cl%d\" -> \"cl%d\"};\n", spawning_id, spawned_id);
}

void graph_spawns(int spawning, spawn_list_t *spawn_list) {
	msg_t msg = msg_new();
	dequeue_iterator_t iterator = dequeue_get_iterator(spawn_list);
	while (dequeue_iterator_has_more(&iterator)) {
		spawn_element_t *spawn_element = (spawn_element_t *)dequeue_iterator_get_next(&iterator);
		msg_cat(&msg, "\t\"cl%d\" -> \"cl%d\";\n", spawning, spawn_element->id);
		msg_cat(&msg, "\t\"cl%d\"[label=\"%s\"];\n", spawn_element->id, spawn_element->label);
	}
	logger_write(logger, msg);
	msg_destroy(msg);
	
}

void graph_send_argument(closure_t *closure, seta_context_t *context) {
	logger_write(logger, "\t\"cl%d\" -> \"cl%d\"[constraint=false, style=dashed];\n",
                 context->closure_id, closure->id);
}

void graph_send_argument_enable(int enabled, char *label) {
	logger_write(logger, "\t\"cl%d\"[label=\"%s\"];\n", enabled, label);
}

void graph_color_S1_element(void *cl, int g) {
	logger_write(logger, "\t\"cl%s\"[fillcolor=yellow];\n", (char *)cl);
}

void graph_finish() {
	logger_write(logger, "}");
	logger_destroy(logger);
}
