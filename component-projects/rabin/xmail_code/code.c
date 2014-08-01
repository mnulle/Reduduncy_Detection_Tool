#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint32_t **TA, **TB, **TC, **TD;

void longLeftShift(uint32_t * sWord, uint32_t len, uint32_t s)
{
	uint32_t temp, i;
	uint32_t mask = 0x00000000;
	// generate mask for extracting s bits
	for (i = 0; i < s; i++)
		mask = mask << 1 | 0x00000001;
	for (i = len - 1; i > 0; i--) {
		// extract s highest bits from lower word
		temp = sWord[i - 1] >> (32 - s) & mask;
		// insert as s lowest bits in higher word
		sWord[i] = sWord[i] << s | temp;
	}
	sWord[0] = sWord[0] << s;
}

void multiT8(uint32_t * sWord, uint32_t len, uint32_t * P)
{
	uint32_t i, j;
	uint32_t needXOR;
	for (i = 0; i < 8; i++)
	{
		// XOR conditioned on MSB
		needXOR = ((sWord[len - 1] & 0x80000000) == 0x80000000);
		// left shift 1 bit
		longLeftShift(sWord, len, 1);
		if( needXOR )
		{
			for (j = 0; j < len; j++)
			{
				sWord[j] = sWord[j] ^ P[j];
			}
		}
	}
}

void byteShift(uint32_t *sWord, uint32_t len, uint32_t i)
{
	uint32_t j;
	sWord[len - 1] = i << 24;
	for (j = 0; j < len - 1; j++) {
		sWord[j] = 0x00000000;
	}
}

void computeTable( uint32_t * P , uint32_t len) {
	uint32_t i, j;
	for (i = 0; i < 256; i++) {
		for (j = 0; j < len; j++) {
			TD[i][j] = 0;
		}
		byteShift(TD[i], len, i); // TD[i] = i × tk−8
		multiT8(TD[i], len, P); // TD[i] = i×t8
		for (j = 0; j < len; j++) {
			TC[i][j] = TD[i][j];
		}
		multiT8(TC[i], len, P);
		for (j = 0; j < len; j++) {
			TB[i][j] = TC[i][j];
		}
		multiT8(TB[i], len, P);
		for (j = 0; j < len; j++) {
			TA[i][j] = TB[i][j];
		}
		multiT8(TA[i], len, P);
	}
}

uint32_t main() {
	int i, len = 1;
	TA = malloc(sizeof(uint32_t*) * 256);
	TB = malloc(sizeof(uint32_t*) * 256);
	TC = malloc(sizeof(uint32_t*) * 256);
	TD = malloc(sizeof(uint32_t*) * 256);
	for(i = 0; i < 256; i++) {
		TA[i] = malloc(sizeof(uint32_t) * len);
		TB[i] = malloc(sizeof(uint32_t) * len);
		TC[i] = malloc(sizeof(uint32_t) * len);
		TD[i] = malloc(sizeof(uint32_t) * len);
	}
	uint32_t* p = malloc(sizeof(uint32_t));
	p[0] = 8396801;
	computeTable(p, 1);
	return 0;
}
