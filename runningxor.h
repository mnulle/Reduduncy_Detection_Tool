/* This Fingerprinter class relies on a very simple running xor fingerprinting.
 * Used just for testing, since it's simple enough to be clearly right.
 */

#ifndef RUNNINGXOR_CLASS /* RUNNINGXOR_CLASS */
#define RUNNINGXOR_CLASS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "polynomial.h"
#include "jfuncs.h"

// All public and some private members are the same as in rabin.h
// in order to make them interchangeable. Proper documentation
// found there.
typedef unsigned int uint;

class Fingerprinter  {
public:
	Fingerprinter();
	Fingerprinter(short k, int win, uint* Pinit);
	Fingerprinter(short k, int win);
	~Fingerprinter();
	uint* fingerprintAll(char S[1600]);
	uint* fpn(char *S, int offset, int n);
	uint* fpn(uint *current, char *S, int offset, int n);
	uint* fp1shift(uint *current, char add, char remove);

	int getWindow();
	short getWCount();

private:
	int window;
	short wCount;
};

#endif	/* RUNNINGXOR_CLASS */
