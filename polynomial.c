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

uint32_t* poly_mod(uint32_t* f, uint32_t* p, int len, int plen) {
    int i, j, k, need; 						// loop counters
    for(k = len-1; k > plen-1; k-= plen) {
	for(i = ((sizeof(uint32_t) * 8) * plen)-1; i >= 0; i--) {	// loop through a full cycle
	    need = f[len-1] & 0x80000000;			// Get MSB of f
		// 0x80000000 = 8*2^28
	    shift_array(f, len);				// shift the array left
	    if(need) {						// if the MSB was 1
		for(j = 0; j < plen; j++)			// xor p with the first plen words of f
		    f[len-1-j] = f[len-1-j] ^ p[plen-1-j];
	    }
	}
    }
    for(j = plen; j < len; j++) {
	for(int i = 0; i < len - 1; i++)
            f[i] = f[i+1];
	f[len-1] = 0;
    }
    return f;							// f now contains the desired value
}

bool keep_going(uint32_t* f, int len, int plen) {
    int i;							// loop counter
    for(i = 0; i < len-plen; i++) {				// check to see if any words after the first plen contain 1's
	if(f[i] != 0) return true;				// if so, return true. We need to get those out
    }
    return false;						// if not, we've finally reduced the polynomial
}

uint32_t* shift_array(uint32_t* f, int len) {
    int i;							// loop counter
    for(i = len-1; i > 0; i--) {				// For every word in f but the last
        f[i] = f[i] << 1;					// shift it left one
	if(f[i-1] & 0x80000000) f[i]++;				// grab the msb from the next word
    }
    f[0] = f[0] << 1;						// shift the last word once, 0 as padding
    return f;
}
