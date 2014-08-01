#include "rabin.h"
#include "jfuncs.h"
#include <time.h>
#include <stdint.h>

int main() {
	int fpBits = 64;
	int window = 16;
	initRabin(fpBits, window);
	unsigned int *fp = fingerprint("abcdefghijklmnopqrstuvwxyz");
	printf("%X %X\n", fp[1], fp[0]);
	fp1shift(fp, 'q', 'a');
	printf("%X %X\n", fp[1], fp[0]);
	fp = fingerprint("bcdefghijklmnopqrstuvwxyz");
	printf("%X %X\n", fp[1], fp[0]);
	closeOutRabin();

	uint32_t hash = hashword((uint32_t*)fp, fpBits/32, 12345678);

	printf("Hashed value: %u\n", hash);

	free (fp);
	return 0;
}
