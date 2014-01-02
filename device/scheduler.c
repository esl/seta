/*
 *  scheduler.c
 *  seta_xcode
 *
 *  Created by Fabio Pricoco on 21/11/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include "seta_internal.h"

extern rowcol_t device_rowcol;

extern device_in;

processor_t *local_proc;

void entry(seta_context_t);

seta_cont_t seta_create_cont(void *arg, seta_handle_spawn_next_t hsn) {
    seta_cont_t cont;
    cont.closure = hsn.closure;
    cont.arg = arg;
    ((closure_t *)hsn.closure)->join_counter += 1;
    return cont;
}

void * seta_alloc_args(long size) {
    return dmalloc(size);
}

void * seta_get_args(seta_context_t *context) {
    return context->args;
}

void seta_free_args(seta_context_t *context) {
    //context->free_args_done = true;
    dfree(context->args);
}

seta_handle_spawn_next_t seta_prepare_spawn_next(void *fun, void *args, seta_context_t *context) {
    seta_handle_spawn_next_t hsn;
    closure_t *closure = closure_create();
    closure_set_fun(closure, fun);
    closure_set_args(closure, args);
    closure->level = context->level;
    closure->core = device_rowcol;
    hsn.closure = (void *)closure;
    return hsn;
}

void seta_spawn_next(seta_handle_spawn_next_t hsn) {
    closure_t *closure = (closure_t *)hsn.closure;
}

void seta_spawn(void *fun, void *args, seta_context_t *context) {
    closure_t *closure = closure_create();
    closure_set_fun(closure, fun);
    closure_set_args(closure, args);
    closure->level = context->level + 1;
    processor_lock_ready_queue(device_rowcol);
    ready_queue_post_closure_to_level(local_proc->rq, closure, closure->level);
    processor_unlock_ready_queue(device_rowcol);
}

void seta_send_argument(seta_cont_t *cont, void *src, int size, seta_context_t *context) {
    if (cont == NULL) {
        //debug("mission complete");
        device_stop_and_send_result(src, size);
        return;
    }
    closure_t *closure = (closure_t *)cont->closure;

    void *dst = cont->arg;
    memcpy(dst, src, size);

    bool to_post = false;
    closure_lock(closure);
    closure->join_counter -= 1;
    if (closure->join_counter == 0) {
        to_post = true;
    }
    closure_unlock(closure);

    if (to_post) {
        processor_lock_ready_queue(closure->core);
        ready_queue_post_closure_to_level(local_proc->rq, closure, closure->level);
        processor_unlock_ready_queue(closure->core);
    }
}

void scheduler_execute_closure(closure_t *closure) {
    seta_context_t context;
    context.args = closure->args;
    context.level = closure->level;
    void (*user_fun)(seta_context_t) = closure->fun;
    closure_destroy(closure);
    user_fun(context);
}
typedef struct _args_fib {
    seta_cont_t k;
    int n;
} args_fib_t;

void scheduler_scheduling_loop() {
    closure_t *closure, *rclosure;
    rowcol_t rowcol;

    while (1) {
        //debug(".0x%x", local_proc->rq);
        processor_lock_ready_queue(device_rowcol);
        closure = ready_queue_extract_head_from_deepest_level(local_proc->rq);
        processor_unlock_ready_queue(device_rowcol);
        if (closure != NULL) {
            //debug("0x%x", closure);
            scheduler_execute_closure(closure);
        }
        else {
            //if (device_is_stopped()) {
            //    break;
            //}
            // stealing...
            do {
                device_get_random_core(&rowcol);
                //debug("%d, %d", rowcol.r, rowcol.c);
            } while(rowcol.r == device_rowcol.r && rowcol.c == device_rowcol.c);
            //debug("%d, %d", rowcol.r, rowcol.c);
            //if (rowcol.r != 1 && rowcol.c != 1) {
            //    debug("%d, %d", rowcol.r, rowcol.c);
            //}

            processor_t *remote_proc =
                *(processor_t **)
                (e_get_global_address(rowcol.r, rowcol.c, &local_proc));
            processor_lock_ready_queue(rowcol);
            rclosure = ready_queue_extract_tail_from_shallowest_level(remote_proc->rq);
            processor_unlock_ready_queue(rowcol);

            if (rclosure != NULL) {
                scheduler_execute_closure(rclosure);
            }
        }
    }
}

void scheduler_start() {
    //creates the local processor
    local_proc = processor_create();

    if (device_rowcol.r == 0 && device_rowcol.c == 0) {
        //insert the initial closure in the processor 0
        closure_t *closure = closure_create();

        void *args = seta_alloc_args(4);
        memcpy(args, &device_in, 4);
        closure_set_args(closure, args);

        closure_set_fun(closure, &entry);
        ready_queue_post_closure_to_level(local_proc->rq, closure, 0);
    }

    //waits for the other processors from being created
    device_sync();

    if (device_rowcol.r == 0 && device_rowcol.c == 0) {
        while(1) {}
    }
    if (device_rowcol.r == 0 && device_rowcol.c == 2) {
        while(1) {}
    }
    if (device_rowcol.r == 0 && device_rowcol.c == 3) {
        while(1) {}
    }

    scheduler_scheduling_loop();
    //device_sync();
    processor_destroy(local_proc);
}
