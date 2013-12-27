// device

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <e_lib.h>

#include "shared.h"
#include "seta_internal.h"

shared_t shared SECTION("shared_dram");

int device_in;

void *resultbuf;

e_barrier_t bar_array[4];
e_barrier_t *tgt_bar_array[4];

e_mutex_t mutex_w;
e_mutex_t mutex_malloc;
e_mutex_t mutex_rq;
e_mutex_t mutex_closure;

rowcol_t device_rowcol;

int rndlike = 1;

void *dmalloc(int n) {
    int i;
    void *m;
    e_mutex_lock(0, 0, &mutex_malloc);
    m = malloc(n);
    for(i=0;i<10000;i++); // wait for malloc to have done
    e_mutex_unlock(0, 0, &mutex_malloc);
    return m;
}

void dfree(void *ptr) {
    int i;
    e_mutex_lock(0, 0, &mutex_malloc);
    free(ptr);
    for(i=0;i<10000;i++); // wait for free to have done
    e_mutex_unlock(0, 0, &mutex_malloc);
}

void device_get_random_core(rowcol_t *rowcol) {
    rowcol->r = 0;
    rowcol->c = 0;
    //rowcol->r = rand() % e_group_config.group_rows;
    //rowcol->c = rand() % e_group_config.group_cols;
    //debug("rowcol: %d %d", rowcol->r, rowcol->c);
}

void debug(const char *format, ...) {
    int i;
    va_list arglist;
    e_mutex_lock(0, 0, &mutex_w);
    va_start(arglist, format);
    vsprintf(shared.out, format, arglist);
    va_end(arglist);
    shared.ready = true;
    for(i=0; i<1000000; i++) {}
    e_mutex_unlock(0, 0, &mutex_w);
}

void device_stop_and_send_result(void *src, int size) {
    int i;
    e_mutex_lock(0, 0, &mutex_w);
    memcpy(shared.out, src, size);
    shared.ready = true;
    // prevents any other processor from writing on "resultbuf" before
    // the host has readed and saved the value.
    for(i=0; i<1000000; i++) {}
    e_mutex_unlock(0, 0, &mutex_w);
}

//bool device_is_stopped() {
//    // seems not working
//    return readybuf;
//}

void device_sync() {
    e_barrier(bar_array, tgt_bar_array);
}

void device_mutex_lock(rowcol_t rowcol, int mutex) {
    if (mutex == MUTEX_RQ) {
        e_mutex_lock(rowcol.r, rowcol.c, &mutex_rq);
    }
    if (mutex == MUTEX_CLOSURE) {
        e_mutex_lock(rowcol.r, rowcol.c, &mutex_closure);
    }
}

void device_mutex_unlock(rowcol_t rowcol, int mutex) {
    if (mutex == MUTEX_RQ) {
        e_mutex_unlock(rowcol.r, rowcol.c, &mutex_rq);
    }
    if (mutex == MUTEX_CLOSURE) {
        e_mutex_unlock(rowcol.r, rowcol.c, &mutex_closure);
    }
}

int main(void) {
    int row, col, i;

    //in_size = shared.in_size;
    //in = shared.in;

    memcpy(&device_in, shared.in, shared.in_size);

    //device_stop_and_send_result(&in, shared.in_size);
    //return;

    row = e_group_config.core_row;
    col = e_group_config.core_col;
    device_rowcol.r = row;
    device_rowcol.c = col;

    e_barrier_init(bar_array, tgt_bar_array);
    if (row == 0 && col == 0) {
        srand(5);
        e_mutex_init(0, 0, &mutex_w, MUTEXATTR_DEFAULT);
        e_mutex_init(0, 0, &mutex_malloc, MUTEXATTR_DEFAULT);
    }
    e_mutex_init(row, col, &mutex_rq, MUTEXATTR_DEFAULT);
    e_mutex_init(row, col, &mutex_closure, MUTEXATTR_DEFAULT);

    scheduler_start();

    return EXIT_SUCCESS;
}

