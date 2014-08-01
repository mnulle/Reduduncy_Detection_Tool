/***************************************************************
 * File: polynomial.c
 * Author: Matt Nulle
 * This file implements the functions found in polynomial.h
 **************************************************************/

#include "polynomial.h"
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>	

#define MAX_VAL ULONG_MAX
//#define MAX_VAL UINT_MAX

uint32_t* poly_mod(uint32_t* f, uint32_t* p, int len, int plen) {
    int i, j; 							// loop counters
    while(keep_going(f, len, plen)) {				// f is not small enough yet
	for(i = 0; i < sizeof(uint32_t) * 8 * plen; i++) {	// loop through a full cycle
	    if(f[0] > MAX_VAL / 2) {				// if the first bit is 1
		for(j = 0; j < plen; j++)			// xor p with the first plen words of f
		    f[j] = f[j] ^ p[j];
	    }
	    shift_array(f, len);				// shift the array left
	}
    }
    if(f[0] > MAX_VAL / 2) {				// check if first bit of f is 1 one more time
	for(j = 0; j < plen; j++)				// xor p with the first plen words of f
	    f[j] = f[j] ^ p[j];
    }
    return f;							// f now contains the desired value
}

bool keep_going(uint32_t* f, int len, int plen) {
    int i;							// loop counter
    for(i = plen; i < len; i++) {				// check to see if any words after the first plen contain 1's
	if(f[i] != 0) return true;				// if so, return true. We need to get those out
    }
    return false;						// if not, we've finally reduced the polynomial
}

uint32_t* shift_array(uint32_t* f, int len) {
    int i;							// loop counter
    for(i = 0; i < len - 1; i++) {				// For every word in f but the last
        f[i] = f[i] << 1;					// shift it left one
	if(f[i+1] > MAX_VAL / 2) f[i]++;			// grab the msb from the next word
    }
    f[len - 1] = f[len - 1] << 1;				// shift the last word once, 0 as padding
    return f;
}
