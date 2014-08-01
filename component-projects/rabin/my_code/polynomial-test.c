#include "polynomial.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    // Declare a couple of polynomial holders
    uint32_t *my_ints;
    uint32_t *p;

    // Initialize polynomial arrays
    p = malloc(sizeof(uint32_t));
    p[0] = (128);

    my_ints = malloc(sizeof(uint32_t)*2);
    my_ints[0] = (128);

    // Print values to make sure it's working
    printf("Size of long is %lu\n", sizeof(uint32_t));
    printf("%X\n", my_ints[0]);
    printf("%X\n", my_ints[1]);
    printf("%X\n", poly_mod(my_ints, p, 2, 2)[0]);
    return 0;
}
