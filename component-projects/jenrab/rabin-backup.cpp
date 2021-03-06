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

// for computing f(first n bytes of S)
void fpn(usInt *fP, usInt *P, char *S, short wCount, int n);

// for computing f(concat(S, 1 byte))
void fp1(usInt *fP, usInt *P, short wCount, char s);
// for computing f(concat(S, 4 bytes))
void fp4(usInt *fP, short wCount, short s1, short s2, short s3, short s4);
// for computing f(shift(S, 1 bytes))
void fp1shift(usInt *fP, usInt *P, short wCount, char s, char x, usInt *tP);
// for computing f(shift(S, 4 bytes))
void fp4shift(usInt *fP, short wCount, short s1, short s2, short s3, short s4, short x1, short x2, short x3, short x4);
// return a random number in the range [0, 1]
double getArandom();

/***** Global Variables *****/
usInt **TA, **TB, **TC, **TD;
usInt* P;
usInt* tP;
int window;
short wCount;	
/***** Application Entry *****/

void initRabin() {
	short k = 64, j;
	wCount = k/32;
	usInt* tn;
	window = 8;
	// generate P
	P = generateIrreduciblePoly(k, &wCount);

	// calculate t^(n-1) mod P
	tn = (usInt *)malloc((window / 4.0)*sizeof(usInt)); //allocate memory
	for(j = 0; j < window/4 - 1; j++)
		tn[j] = 0;
	tn[window/4 - 1] = 0x80000000;
	tP = (usInt *)malloc(k/32*sizeof(usInt));
	tP = &poly_mod(tn, P, window/4, k/32)[window/4 - 1];

	// allocate memory for tables and generate tables
	TA = (usInt **)malloc(256*sizeof(usInt*));
	TA[0] = (usInt *)malloc(256*wCount*sizeof(usInt));
	TB = (usInt **)malloc(256*sizeof(usInt*));
	TB[0] = (usInt *)malloc(256*wCount*sizeof(usInt));
	TC = (usInt **)malloc(256*sizeof(usInt*));
	TC[0] = (usInt *)malloc(256*wCount*sizeof(usInt));
	TD = (usInt **)malloc(256*sizeof(int*));
	TD[0] = (usInt *)malloc(256*wCount*sizeof(usInt));
	if((!TA)||(!TB)||(!TC)||(!TD)){
		fprintf(stderr, "Memory allocate error in main--1\n");
		exit(1);
	}
	if((!TA[0])||(!TB[0])||(!TC[0])||(!TD[0])){
		fprintf(stderr, "Memory allocate error in main--2\n");
		exit(1);
	}
	computTable(TA, TB, TC, TD, P, wCount);


}

void initRabin(short k, int win) {
	short j;
	wCount = k/32;
	usInt* tn;
	window = win;
	// generate P
	P = generateIrreduciblePoly(k, &wCount);

	// calculate t^(n-1) mod P
	tn = (usInt *)malloc((window / 4.0)*sizeof(usInt)); //allocate memory
	for(j = 0; j < window/4 - 1; j++)
		tn[j] = 0;
	tn[window/4 - 1] = 0x80000000;
	tP = (usInt *)malloc(k/32*sizeof(usInt));
	tP = poly_mod(tn, P, window/4, k/32);
	free(tn);

	// allocate memory for tables and generate tables
	TA = (usInt **)malloc(256*sizeof(usInt*));
	TA[0] = (usInt *)malloc(256*wCount*sizeof(usInt));
	TB = (usInt **)malloc(256*sizeof(usInt*));
	TB[0] = (usInt *)malloc(256*wCount*sizeof(usInt));
	TC = (usInt **)malloc(256*sizeof(usInt*));
	TC[0] = (usInt *)malloc(256*wCount*sizeof(usInt));
	TD = (usInt **)malloc(256*sizeof(int*));
	TD[0] = (usInt *)malloc(256*wCount*sizeof(usInt));
	if((!TA)||(!TB)||(!TC)||(!TD)){
		fprintf(stderr, "Memory allocate error in main--1\n");
		exit(1);
	}
	if((!TA[0])||(!TB[0])||(!TC[0])||(!TD[0])){
		fprintf(stderr, "Memory allocate error in main--2\n");
		exit(1);
	}
	computTable(TA, TB, TC, TD, P, wCount);
}


usInt* fingerprint(char url[1600]) {
	short j;
	// actual process
	usInt* fP = (usInt *)malloc(wCount*sizeof(usInt)); // for storing fingerprint
	if(!fP) {
		fprintf(stderr, "Memory allocate error in main--3\n");
		exit(1);
	}

	for ( j = 0; j < wCount; j++ ) fP[j] = 0;
	fpn(fP, P, url, wCount, window);
	
	return fP;
}

void closeOutRabin() {
	free(P); free(tP);
	free(TA[0]); free(TB[0]); free(TC[0]); free(TD[0]);
	free(TA); free(TB); free(TC); free(TD);
}

/***** Methods *****/
usInt * generateIrreduciblePoly(short k, short *size) {
	// precondition
	if((k<0)||(k % 32 != 0)){
		cout<<"Error in Generating Irreducible Polynomial, k - value"<<endl;
		exit(1);
	}
	//Seed the random-number generator with current time so that
	//the numbers will be different every time we run.
	//srand( (unsigned)time( NULL ) );
	double temp;
	for(int n=0; n<5; n++) {
		//I found the first generated random does not change,
		//so I choose the fifth one
		temp = getArandom(); //temp is [0, 1)
	}
	int r = (int) ( temp * (floor((k-1)/2.0)-1));
	r = 2*r + 1; // r is a random odd value within [1, k-2]
	r = 1;
	usInt *A;//A array is going to store the poly. coefficents.
	A = (usInt *)malloc(k*sizeof(usInt)); //allocate memory
	if(!A) {
		cout<<"Memory allocate error for Irreducible Polynomial--1"<<endl;
		exit(1);
	}
	//initialize the array
	A[0] = 1; //A[k-1] = 1;
	A[1] = 1;
	short i;
	for(i=2; i<= k-1; i++) // should be i <= k - 1 instead of i < k - 1
		A[i] = 0;
	/*for(i=1; i<=r; i++){
		while(1){
			temp = getArandom(); //temp is [0, 1)
			short j = (short)(temp * (k-2)); // j is [0, k-2)
			if(A[j+1]==0){
				A[j+1] = 1;
				break;
			}
		}//end of while loop
	}//end of for i loop*/
	*size = k/32;
	usInt *W;
	W = (usInt *)malloc((*size)*sizeof(usInt)); //allocate memory
	if(!W) {
		cout<<"Memory allocatet error for Irreducible Polynomial--2"<<endl;
		exit(1);
	}
	for(short j=0; j<*size; j++){
		W[j] = 0;
		for(i=32*(j+1)-1; i>=32*(j+1)-32; i--)
			W[j] = 2*W[j] + A[i];
	}
	return W;
}

void computTable
(usInt **TA, usInt **TB, usInt **TC, usInt **TD, usInt *P, short wCount) {
	short i;
	for(i=1; i<256; i++) {
		TA[i] = TA[i-1] + wCount; //allocate address
		TB[i] = TB[i-1] + wCount; //allocate address
		TC[i] = TC[i-1] + wCount; //allocate address
		TD[i] = TD[i-1] + wCount; //allocate address
	}
	for(i=0; i<256; i++) {
		for(short j=0; j<wCount; j++) {
			TA[i][j] = 0; //initialize
			TB[i][j] = 0; //initialize
			TC[i][j] = 0; //initialize
			TD[i][j] = 0; //initialize
		}
	}
	for(i=0; i<256; i++) {
		short j;
		for(j=0; j < wCount; j++) {
			TD[i][j] = 0;
		}
		byteShift(TD[i], wCount, i);
		multiT8(TD[i], P, wCount);
		for(j=0; j < wCount; j++) {
			TC[i][j] = TD[i][j];
		}
		multiT8(TC[i], P, wCount);
		for(j=0; j < wCount; j++) {
			TB[i][j] = TC[i][j];
		}
		multiT8(TB[i], P, wCount);
		for(j=0; j < wCount; j++) {
			TA[i][j] = TB[i][j];
		}
		multiT8(TA[i], P, wCount);
	}
}

void multiT8(usInt *words, usInt *P, short size) 
{
	int needXOR;
	short i, j;
	for(i=0; i<8; i++){
		// XOR is needed only when the MSB = 1
		needXOR = ((words[size-1] & 0x80000000) == 0x80000000);
		// left shift 1 bit
		longLeftShift(words, size, 1);
		if ( needXOR ){
			for(j=size-1; j>=0; j--){
				words[j] = words[j] ^ P[j];
			}
		}
	}
}
void byteShift(usInt *words, short len, short value)
{
	short i;
	words[len - 1] = value << 24;
	for(i = len - 2; i >= 0; i--){
		words[i] = 0x00000000;
	}
}
void longLeftShift(usInt *words, short len, short s)
{
	usInt temp;
	usInt mask = 0x00000000;
	short i;
	for (i = 0; i < s; i++) //create a mask for s LSB
		mask = mask << 1 | 0x00000001;
	for (i = len - 1; i > 0; i--) {
		temp = words[i-1] >> (32 - s) & mask; // get a MSB from low
		words[i] = words[i] << s | temp; // set as s LSB in high
	}
	words[0] = words[0] << s; // shift the low word
}

void fpn(usInt *fP, usInt *P, char *S, short wCount, int max) {
	int j, len, n;
	for ( j = 0; j < wCount; j++ ) fP[j] = 0;
	len = strlen(S);
	if(max > len) {
		fprintf(stderr, "Warning: requested length is longer than the string. Fingerprinting the whole string\n");
		fprintf(stderr, "\tlen: %d\n\tmax: %d", len, max);
		max = len;
	}
	n = max / 4;
	for ( j = 1; j <= n; j++ ) {
		fp4(fP, wCount, S[4*j-4], S[4*j-3], S[4*j-2], S[4*j-1]);
	}
	for ( j = 4*n; j < max; j++) {
		fp1(fP, P, wCount, S[j]);
	}
}

void fp1(usInt *fP, usInt *P, short wCount, char s) {
	usInt W;
	int needXOR;
	short i, j;
	for ( i = 0; i < 8; i++ ) {
		W = s & 0x80; // retrieve the MSB of s
		s = s << 1; // shift for next round
		needXOR = ( (fP[wCount-1] & 0x80000000) == 0x80000000 );

		longLeftShift(fP, wCount, 1);

		if ( needXOR ){
			for( j = wCount - 1; j >= 0; j--) {
				fP[j] = fP[j] ^ P[j];
			}
		}
		fP[0] = fP[0] ^ (W >> 7);
	}
}

void fp1shift(usInt *fP, char add, char remove) {
	usInt W = 200, Y = 200;
	int needXOR;
	short i, j;
	for ( i = 0; i < 8; i++ ) {
		W = add & 0x80; // retrieve the MSB of s
		Y = remove & 0x80; // retrieve the MSB of x
		add = add << 1; // shift for next round
		remove = remove << 1;
		if(Y) {
			for( j = wCount - 1; j >= 0; j--) 
				fP[j] = fP[j] ^ tP[j];
			//fP[wCount - 1] = fP[wCount - 1] ^ 0x80000000;
		}
		needXOR = ( (fP[wCount-1] & 0x80000000) == 0x80000000 );
		longLeftShift(fP, wCount, 1);
		if ( needXOR ){
			for( j = wCount - 1; j >= 0; j--) {
				fP[j] = fP[j] ^ P[j];
			}
		}
		fP[0] = fP[0] ^ (W >> 7);
	}
}

void fp4(usInt *fP, short wCount, short s1, short s2, short s3, short s4) {
	usInt W = 0;
	short j;
	unsigned short f1, f2, f3, f4;
	W = (((W | s1) << 8 | s2 ) << 8 | s3 ) << 8 | s4;
	f1 = (fP[wCount - 1] >> 24 ) & 0xff;
	f2 = (fP[wCount - 1] >> 16 ) & 0xff;
	f3 = (fP[wCount - 1] >> 8 ) & 0xff;
	f4 = fP[wCount - 1] & 0xff;
	for( j = wCount - 1; j > 0; j--) {
		fP[j] = fP[j-1] ^ TA[f1][j] ^ TB[f2][j] ^ TC[f3][j] ^ TD[f4][j];
	}
	fP[0] = W ^ TA[f1][0] ^ TB[f2][0] ^ TC[f3][0] ^ TD[f4][0];
}

void fp4shift(usInt *fP, short wCount, short s1, short s2, short s3, short s4, short x1, short x2, short x3, short x4){

}

double getArandom() {
	return rand()/(double)RAND_MAX;
}
