#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <iostream>
#include "polynomial.h"
#include "jfuncs.h"
#include "runningxor.h"

#define DEBUG_PRINT 0

// constructors initialize variables
Fingerprinter::Fingerprinter() {
	short k = 64;
	wCount = k/32;
	window = 16;
	
}

Fingerprinter::Fingerprinter(short k, int win) {
	wCount = k/32;
	window = win;
}

Fingerprinter::Fingerprinter(short k, int win, uint* Pinit) {
	wCount = k/32;
	window = win;
}

// nothing in this one to free
Fingerprinter::~Fingerprinter() {
}

// xor all the characters
uint* Fingerprinter::fingerprintAll(char S[1600]) {
	short j;
	// allocate fingerprint memory
	uint* fP = (uint *)malloc(wCount*sizeof(uint)); // for storing fingerprint
	if(!fP) {
		fprintf(stderr, "Memory allocate error in fingerprintAll\n");
		exit(1);
	}
	// init fingerprint to 0
	for ( j = 0; j < wCount; j++ ) fP[j] = 0;
	// fingerprint with all characters of S
	fpn(fP, S, 0, strlen(S));
	return fP;
}

// Return fingerprint of S, beginning at offset, going max characters or to the end
uint* Fingerprinter::fpn(char *S, int offset, int max) {
	// allocate memory for the fingerprint
	uint *current;
	current = (uint *)malloc((wCount)*sizeof(uint)); //allocate memory
	// fingerprint using the now empty fingerprint
	return fpn(current, S, offset, max);
}

// Fingerprint S into current, and return it
uint* Fingerprinter::fpn(uint *current, char *S, int offset, int max) {
	int j, len;
	// set the fingerprint to 0
	for ( j = 0; j < wCount; j++ ) current[j] = 0;
	// find length of string
	len = strlen(S);
	// make sure we don't go over the length of the string
	if(max + offset > len) {
		if(DEBUG_PRINT) {
			fprintf(stderr, "Warning: requested length after offset is longer than the string. Fingerprinting the whole string\n");
			fprintf(stderr, "\tlen: %d\n\tmax: %d + %d = %d\n", len, max, offset, max+offset);
		}
		max = len - offset;
	}
	// xor the characters into the fingerprint
	for(j = offset; j < offset + max; j++)
		current[0] ^= S[j];
	return current;
}

// shift the fingerprint by one character
uint* Fingerprinter::fp1shift(uint *current, char add, char remove) {
	// xor the new and old characters with the fingerprint
	current[0] ^= remove;
	current[0] ^= add;
	return current;
}

// return window
int Fingerprinter::getWindow() {
	return window;
}

// return wCount
short Fingerprinter::getWCount() {
	return wCount;
}
