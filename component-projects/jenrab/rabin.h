/***************************************************************************
* Authors : Calvin Chan & Haihua Lu
* Course Project: CMPUT 690 -- Advanced Database System, December, 2001
* Instructor : Dr. Davood Rafiei
*
* Filename : fingerPrint.cc
*
* Purpose : to run finger printing test on pages.txt
*
* Usage : fp <degree> <output file name> < input stream
* if no output file name is given, "outfp.txt" will be used
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "polynomial.h"
#include "jfuncs.h"

using namespace std;

/***** Type Definitions *****/
typedef unsigned int usInt;
#define DEBUG 1
/***** Forward Declarations *****/
// for generating irreducible polynomial of degree k
// wCount being the number of 32 bit words required to hold P(t)
usInt* generateIrreduciblePoly(short k, short *wCount);
// for generating pre-computed table TA, TB, TC, TD
void computTable

(usInt **TA, usInt **TB, usInt **TC, usInt **TD, usInt *P, short wCount);
// for left shifting values s bits
void longLeftShift(usInt *words, short wCount, short s);
// for computing i*t^8 mod P(t)
void multiT8(usInt *words, usInt *P, short wCount);
// for computing i*t^(k-8) mod P(t)
void byteShift(usInt *words, short wCount, short value);

// To initialize tables and values and such
void initRabin(short k, int win);

// for computing f(first 'window' bytes of S)
usInt* fingerprint(char url[1600]);

// To free memory associated with the globals 
void closeOutRabin();

// for computing f(first n bytes of S)
void fpn(usInt *fP, usInt *P, char *S, short wCount, int n);

// for computing f(concat(S, 1 byte))
void fp1(usInt *fP, usInt *P, short wCount, char s);
// for computing f(concat(S, 4 bytes))
void fp4(usInt *fP, short wCount, short s1, short s2, short s3, short s4);
// for computing f(shift(S, 1 bytes))
void fp1shift(usInt *fP, char add, char remove);
// return a random number in the range [0, 1]
double getArandom();
