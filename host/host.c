// host

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>

#include <e-hal.h>

#include "shared.h"

#define BUFOFFSET (0x01000000)

int input;

int seta_run(int x, int y, int nx, int ny, void *in, int in_size, void *fun, int out_size) {
    unsigned row, col, coreid, i;
    e_epiphany_t dev;
    e_mem_t emem;
    bool ready;
    void *result;

    e_init(NULL);
    e_reset_system();

    e_alloc(&emem, BUFOFFSET, sizeof(shared_t));
    e_open(&dev, x, y, nx, ny);
    // Reset the workgroup
    for (row=x; row<nx; row++) {
        for(col=y; col<ny; col++) {
            printf("reset core:(%d,%d)  ", row, col);
            e_reset_core(&dev, row, col);
        }
    }
    printf("\n");
    printf("running \"e_seta\" on workgroup (%d, %d, %d, %d)\n", x, y, nx, ny);
    e_load_group("e_seta.srec", &dev, x, y, nx, ny, E_FALSE);

    //init
    e_write(&emem, 0, 0, (off_t)offsetof(shared_t, in_size), &in_size, sizeof(int));
    e_write(&emem, 0, 0, (off_t)offsetof(shared_t, in), in, in_size);

    e_start_group(&dev);

    ready = false;

    result = malloc(out_size);

    do {
        e_read(&emem, 0, 0, (off_t)offsetof(shared_t, ready), &ready, sizeof(bool));
    } while(!ready);
    e_read(&emem, 0, 0, (off_t)offsetof(shared_t, out), result, out_size);

    void (*cb)(void *) = fun;
    cb(result);
    free(result);

    e_close(&dev);

    e_free(&emem);
    e_finalize();

    return 0;
}

void fun(void *result) {
    printf("fibonacci(%d): \"%d\"\n", input, *(int *)result);
    //printf("debug: \"%s\"\n", (char *)result);
}

int main(int argc, char *argv[]) {
    int f = atoi(argv[1]);
    input = f;

    // Assumption: int is 4 bytes on both device (Epiphany) and host (ARM)
    seta_run(0, 0, 2, 2, &f, sizeof(int), &fun, 4);

    return 0;
}
