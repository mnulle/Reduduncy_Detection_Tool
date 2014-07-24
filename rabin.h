/* This class is a fingerprinter which uses the rabin fingerprinting method.
 * Rabin's method described at http://www.xmailserver.org/rabin.pdf
 * This code was modified from http://www.xmailserver.org/rabin_impl.pdf
 * Author: Matt Nulle
 * Date: June 2014
 */

#ifndef RABIN_CLASS /* RABIN_CLASS */
#define RABIN_CLASS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "polynomial.h"
#include "jfuncs.h"

// typedef for brevity
typedef unsigned int uint;

// Fingerprint class
class Fingerprinter  {
public:
	// Constructors
	Fingerprinter();
	// Provide length of fingerprint in bytes, length of window, and P
	Fingerprinter(short k, int win, uint* Pinit);
	// Provide length of fingerprint in bytes, and length of window	
	Fingerprinter(short k, int win);
	// Free all memory used by fingerprinter
	~Fingerprinter();

	// Fingerprint the entire string at once
	uint* fingerprintAll(char S[1600]);
	// Fingerprint n characters of S starting at offset. Allocates memory for fingerprint
	uint* fpn(char *S, int offset, int n);
	// Fingerprint into current n characters of S starting at offset 
	uint* fpn(uint *current, char *S, int offset, int n);

	// Shift the current fingerprint by adding and removing one character
	uint* fp1shift(uint *current, char add, char remove);

	// Return the window (necessary for calculating t^n
	int getWindow();
	// Return wCount, size of fingerprint in words
	short getWCount();

private:
	// Theoretically used to make a random polynomial for use, not used
	uint* generateIrreduciblePoly();
	// Compute TA, TB, etc. 
	void computeTables();
	// Shift words (len uints in words) to the left by amount
	void longLeftShift(uint *words, short len, short amount);
	// Shift words (len uints in words) right by amount
	void longRightShift(uint *words, short len, short amount);
	// Used for calculating tables
	void multiT8(uint *words);
	// put value at highest value in words (I think, function copied)
	void byteShift(uint *words, short len, short value);
	// add 1 bytes to the fingerprint
	uint* fp1(uint *current, char add);
	// add 4 bytes to the fingerprint (quicker)
	uint* fp4(uint *current, short s1, short s2, short s3, short s4);
	// get a random number between 0 and 1
	double getArandom();

	// P (polynomial used for fingerprinting)
	// tP (t^n % P)
	uint *P, *tP;
	// Tables used for fp4
	uint **TA, **TB, **TC, **TD;
	
	// number of bytes fingerprinted at a time
	int window;
	// size of fingerprint in words
	short wCount;
};

#endif	/* RABIN_CLASS */
