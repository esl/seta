#include <stdbool.h>

// The __packed__ attribute is needed by the Epiphany device program
typedef struct __attribute__((__packed__)) {
    bool ready;
    int in_size;
    char in[500];
    char out[500];
} shared_t;
