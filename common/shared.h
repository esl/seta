#include <stdbool.h>

// The __packed__ attribute is needed by the Epiphany device program
typedef struct __attribute__((__packed__)) {
    int in_size;
    char in[10];
    bool out_ready;
    char out[10];
} shared_t;
