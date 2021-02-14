#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "helpers.h"

void init_program() {
    srand(time(NULL)); // NOLINT(cert-msc51-cpp)
}

void *safe_malloc(size_t n) {
    void *p = malloc(n);
    if (p == NULL) {
        kill_with_int("FATAL ERROR! FAILED TO ALLOCATE", n, " BYTES IN MEMORY!");
    }
    return p;
}

void *safe_realloc(void *old_ptr, size_t n) {
    void *p = realloc(old_ptr, n);
    if (p == NULL) {
        kill_with_int("FATAL ERROR! FAILED TO ALLOCATE", n, " BYTES IN MEMORY!");
    }
    return p;
}

int rand_int_in_range(int lower, int upper) {
    return rand() % (upper - lower + 1) + lower; // NOLINT(cert-msc50-cpp)
}

void shuffle_int_array(int *arr, int n) {
    int i, swap, temp;
    for (i = n - 1; i > 0; i--) {
        swap = rand_int_in_range(0, i);
        temp = arr[i];
        arr[i] = arr[swap];
        arr[swap] = temp;
    }
}

void kill(char *reason) {
    printf("%s\n", reason);
    exit(1);
}

void kill_with_int(char *reason_start, int number, char *reason_end) {
    printf("%s %d %s\n", reason_start, number, reason_end);
    exit(1);
}









