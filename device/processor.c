//
//  processor.c
//  seta
//
//  Created by Fabio Pricoco on 19/10/13.
//  Copyright (c) 2013 Erlang Solution Ltd. All rights reserved.
//

#include "seta_internal.h"
#include <stdlib.h>

extern rowcol_t device_rowcol;

processor_t * processor_create() {
    processor_t *proc = (processor_t *)dmalloc(sizeof(processor_t));
    proc->rq = ready_queue_create();
    return proc;
}

void processor_destroy(processor_t *proc) {
    ready_queue_destroy(proc->rq);
    dfree(proc);
}

void processor_lock_ready_queue(rowcol_t rowcol) {
    device_mutex_lock(rowcol, MUTEX_RQ);
}

void processor_unlock_ready_queue(rowcol_t rowcol) {
    device_mutex_unlock(rowcol, MUTEX_RQ);
}
