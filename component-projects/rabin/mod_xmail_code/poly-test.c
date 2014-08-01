#include "polynomial.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
	unsigned int *f , *p;
 	f = (unsigned int*) malloc(sizeof(unsigned int) * 2);
	f[1] = 0x85858585;
	f[0] = 0x85858585;
	p = (unsigned int*) malloc(sizeof(unsigned int));
	p[0] = 0x00000003;
	f = poly_mod(f, p, 2, 1);
	printf("f = [1]%X [2]%X\n", f[1], f[0]);
	return 0;
}
