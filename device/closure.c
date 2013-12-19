#include <stdlib.h>
#include "seta_internal.h"

int closure_id = 0;

closure_t * closure_create() {
    closure_t *closure = (closure_t *)dmalloc(sizeof(closure_t));
    closure->fun = NULL;
    closure->level = 0;
    closure->join_counter = 0;
    closure->args = NULL;
    return closure;
}

void closure_destroy(closure_t *closure) {
    dfree(closure);
}

void closure_set_fun(closure_t *cl, void *fun) {
    cl->fun = fun;
}

void closure_set_args(closure_t *cl, void *args) {
    cl->args = args;
}

void closure_lock(closure_t *cl) {
    device_mutex_lock(cl->core, MUTEX_CLOSURE);
}

void closure_unlock(closure_t *cl) {
    device_mutex_unlock(cl->core, MUTEX_CLOSURE);
}
