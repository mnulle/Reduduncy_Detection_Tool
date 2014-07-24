#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <iostream>
#include "polynomial.h"
#include "jfuncs.h"
#include "rabin.h"

#define DEBUG_PRINT 0

Fingerprinter::Fingerprinter() {
	short k = 64, j;
	wCount = k/32;
	window = 16;
	// generate P
	P = generateIrreduciblePoly();

	// calculate t^(n-1) mod P
	tP = (uint *)malloc((window / 4.0)*sizeof(uint)); //allocate memory
	for(j = 0; j < window/4 - 1; j++)
		tP[j] = 0;
	tP[window/4 - 1] = 0x80000000;
	//tP = (uint *)malloc(k/32*sizeof(uint));
	tP = poly_mod(tP, P, window/4, k/32);

	// allocate memory for tables and generate tables
	TA = (uint **)malloc(256*sizeof(uint*));
	TA[0] = (uint *)malloc(256*wCount*sizeof(uint));
	TB = (uint **)malloc(256*sizeof(uint*));
	TB[0] = (uint *)malloc(256*wCount*sizeof(uint));
	TC = (uint **)malloc(256*sizeof(uint*));
	TC[0] = (uint *)malloc(256*wCount*sizeof(uint));
	TD = (uint **)malloc(256*sizeof(int*));
	TD[0] = (uint *)malloc(256*wCount*sizeof(uint));
	if((!TA)||(!TB)||(!TC)||(!TD)){
		fprintf(stderr, "Memory allocate error in main--1\n");
		exit(1);
	}
	if((!TA[0])||(!TB[0])||(!TC[0])||(!TD[0])){
		fprintf(stderr, "Memory allocate error in main--2\n");
		exit(1);
	}
	computeTables();
}

Fingerprinter::Fingerprinter(short k, int win) {
	short j;
	wCount = k/32;
	window = win;
	// generate P
	P = generateIrreduciblePoly();

	// calculate t^(n-1) mod P
	tP = (uint *)malloc((window / 4.0)*sizeof(uint)); //allocate memory
	for(j = 0; j < window/4 - 1; j++)
		tP[j] = 0;
	tP[window/4 - 1] = 0x80000000;
	//tP = (uint *)malloc(k/32*sizeof(uint));
	tP = poly_mod(tP, P, window/4, k/32);
	for(int i = 0; i < (window / 4) - 1; i++)
		tP[i] = tP[i+1];

	// allocate memory for tables and generate tables
	TA = (uint **)malloc(256*sizeof(uint*));
	TA[0] = (uint *)malloc(256*wCount*sizeof(uint));
	TB = (uint **)malloc(256*sizeof(uint*));
	TB[0] = (uint *)malloc(256*wCount*sizeof(uint));
	TC = (uint **)malloc(256*sizeof(uint*));
	TC[0] = (uint *)malloc(256*wCount*sizeof(uint));
	TD = (uint **)malloc(256*sizeof(int*));
	TD[0] = (uint *)malloc(256*wCount*sizeof(uint));
	if((!TA)||(!TB)||(!TC)||(!TD)){
		fprintf(stderr, "Memory allocate error in main--1\n");
		exit(1);
	}
	if((!TA[0])||(!TB[0])||(!TC[0])||(!TD[0])){
		fprintf(stderr, "Memory allocate error in main--2\n");
		exit(1);
	}
	computeTables();
}

Fingerprinter::Fingerprinter(short k, int win, uint* Pinit) {
	short j;
	wCount = k/32;
	window = win;
	// init P
	P = Pinit;

	// calculate t^(n-1) mod P
	tP = (uint *)malloc((window / 4.0)*sizeof(uint)); //allocate memory
	for(j = 0; j < window/4 - 1; j++)
		tP[j] = 0;
	tP[window/4 - 1] = 0x80000000;
	//tP = (uint *)malloc(k/32*sizeof(uint));
	tP = poly_mod(tP, P, window/4, k/32);

	// allocate memory for tables and generate tables
	TA = (uint **)malloc(256*sizeof(uint*));
	TA[0] = (uint *)malloc(256*wCount*sizeof(uint));
	TB = (uint **)malloc(256*sizeof(uint*));
	TB[0] = (uint *)malloc(256*wCount*sizeof(uint));
	TC = (uint **)malloc(256*sizeof(uint*));
	TC[0] = (uint *)malloc(256*wCount*sizeof(uint));
	TD = (uint **)malloc(256*sizeof(int*));
	TD[0] = (uint *)malloc(256*wCount*sizeof(uint));
	if((!TA)||(!TB)||(!TC)||(!TD)){
		fprintf(stderr, "Memory allocate error in main--1\n");
		exit(1);
	}
	if((!TA[0])||(!TB[0])||(!TC[0])||(!TD[0])){
		fprintf(stderr, "Memory allocate error in main--2\n");
		exit(1);
	}
	computeTables();
}

// Release memory allocated
Fingerprinter::~Fingerprinter() {
	free(P); 
	free(tP);
	free(TA[0]); 
	free(TB[0]); 
	free(TC[0]); 
	free(TD[0]);
	free(TA); 
	free(TB); 
	free(TC); 
	free(TD);
}

// Fingerprints all the characters of S
uint* Fingerprinter::fingerprintAll(char S[1600]) {
	short j;
	// allocate memory for fingerprint
	uint* fP = (uint *)malloc(wCount*sizeof(uint)); // for storing fingerprint
	if(!fP) {
		fprintf(stderr, "Memory allocate error in main--3\n");
		exit(1);
	}
	// set fingerprint to 0
	for ( j = 0; j < wCount; j++ ) fP[j] = 0;
	// fingerprint all characters
	fpn(fP, S, 0, strlen(S));
	return fP;
}

// Beginning at offset, fingerprint max characters of S, or until end of string
uint* Fingerprinter::fpn(char *S, int offset, int max) {
	// allocate memory for fingerprint, since not given it
	uint *current;
	current = (uint *)malloc((wCount)*sizeof(uint));
	// pass to the function that does work
	return fpn(current, S, offset, max);
}

uint* Fingerprinter::fpn(uint *current, char *S, int offset, int max) {
	int j, len, n;

	// set the fingerprint to 0
	for ( j = 0; j < wCount; j++ ) current[j] = 0;

	// get the length of the string
	len = strlen(S);
	// make sure we're not trying to read past the end of S
	if(max + offset > len) {
		if(DEBUG_PRINT) {
			fprintf(stderr, "Warning: requested length after offset is longer than the string. Fingerprinting the whole string\n");
			fprintf(stderr, "\tlen: %d\n\tmax: %d + %d = %d\n", len, max, offset, max+offset);
		}
		max = len - offset;
	}

	// figure out how many times we can call fp4
	n = (offset / 4) + (max / 4);
	// Add 4 bytes at a time
	for ( j = offset / 4 + 1; j <= n; j++ ) {
		fp4(current, S[4*j-4+offset%4], S[4*j-3+offset%4], S[4*j-2+offset%4], S[4*j-1+offset%4]);
	}
	// Add the last couple bytes
	for ( j = 4*n + (offset % 4); j < max + offset; j++) {
		fp1(current, S[j]);
	}

	return current;
}

// Add one character and remove one from the fingerprint
uint* Fingerprinter::fp1shift(uint *current, char add, char remove) {	
	uint W = 200, Y = 200;
	int needXOR;
	short i, j;

	// Move it one bit at a time
	for ( i = 0; i < 8; i++ ) {
		W = add & 0x80; // retrieve the MSB of s
		Y = remove & 0x80; // retrieve the MSB of x
		
		add = add << 1; // shift for next round
		remove = remove << 1;

		// if the highest remaining bit of the byte being removed is 1
		if(Y) {
			// xor with tP to remove it
			for( j = 0; j < wCount; j++) 
				current[j] = current[j] ^ tP[j];
			//current[wCount - 1] = current[wCount - 1] ^ 0x80000000;
		}
		// get the highest bit of the leftover set up
		needXOR = ( (current[wCount-1] & 0x80000000) == 0x80000000 );
		// shift left
		longLeftShift(current, wCount, 1);
		
		// if we just shifted off a 1, need to xor with P
		if ( needXOR ){
			for( j = 0; j < wCount; j++) {
				current[j] = current[j] ^ P[j];
			}
		}
		// Finally, xor the bit of add onto the end
		current[0] = current[0] ^ (W >> 7);
	}
	return current;
}

// Get window
int Fingerprinter::getWindow() {
	return window;
}

// Get wCount
short Fingerprinter::getWCount() {
	return wCount;
}

// Theoretically makes a random irreducible polynomial. The polys generated weren't working, unknown reason
uint* Fingerprinter::generateIrreduciblePoly() {
	int k = wCount * 32;	
	// precondition
	if((k<0)||(k % 32 != 0)){
		std::cout<<"Error in Generating Irreducible Polynomial, k - value"<<std::endl;
		exit(1);
	}
	//Seed the random-number generator with current time so that
	//the numbers will be different every time we run.
	srand( (unsigned)time( NULL ) );
	double temp;
	for(int n=0; n<5; n++) {
		//I found the first generated random does not change,
		//so I choose the fifth one
		temp = getArandom(); //temp is [0, 1)
	}
	int r = (int) ( temp * (floor((k-1)/2.0)-1));
	r = 2*r + 1; // r is a random odd value within [1, k-2]
	r = 1;
	uint *A;//A array is going to store the poly. coefficents.
	A = (uint *)malloc(k*sizeof(uint)); //allocate memory
	if(!A) {
		std::cout<<"Memory allocate error for Irreducible Polynomial--1"<<std::endl;
		exit(1);
	}
	//initialize the array
	A[0] = 1; //A[k-1] = 1;

	short i;
	for(i=1; i<= k-1; i++) // should be i <= k - 1 instead of i < k - 1
		A[i] = 0;
	for(i=1; i<=r; i++){
		while(1){
			temp = getArandom(); //temp is [0, 1)
			short j = (short)(temp * (k-2)); // j is [0, k-2)
			if(A[j+1]==0){
				A[j+1] = 1;
				break;
			}
		}//end of while loop
	}//end of for i loop
	uint *W;
	W = (uint *)malloc((wCount)*sizeof(uint)); //allocate memory
	if(!W) {
		std::cout<<"Memory allocate error for Irreducible Polynomial--2"<<std::endl;
		exit(1);
	}
	for(short j=0; j<wCount; j++){
		W[j] = 0;
		for(i=32*(j+1)-1; i>=32*(j+1)-32; i--)
			W[j] = 2*W[j] + A[i];
	}
	free(A);
	return W;
}

// Compute the tables used for fp4
void Fingerprinter::computeTables() {
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
		multiT8(TD[i]);
		for(j=0; j < wCount; j++) {
			TC[i][j] = TD[i][j];
		}
		multiT8(TC[i]);
		for(j=0; j < wCount; j++) {
			TB[i][j] = TC[i][j];
		}
		multiT8(TB[i]);
		for(j=0; j < wCount; j++) {
			TA[i][j] = TB[i][j];
		}
		multiT8(TA[i]);
	}
}

// Shift the words (len # of uints) by amount bits to the left
void Fingerprinter::longLeftShift(uint *words, short len, short amount) {
	uint temp;
	uint mask = 0x00000000;
	short i;

	for (i = 0; i < amount; i++) //create a mask for amount LSB
		mask = mask << 1 | 0x00000001;

	for (i = len - 1; i > 0; i--) {
		temp = words[i-1] >> (32 - amount) & mask; // get a MSB from low
		words[i] = words[i] << amount | temp; // set as amount LSB in high
	}

	words[0] = words[0] << amount; // shift the low word
}

// Shift the words (len # of uints) by amount bits to the right
void Fingerprinter::longRightShift(uint *words, short len, short amount) {
	uint temp;
	uint mask = 0x00000000;
	short i;

	for (i = 0; i < amount; i++) //create a mask for amount LSB
		mask = mask >> 1 | 0x80000000;

	for (i = 0; i < len - 1; i++) {
		temp = words[i+1] << (32 - amount) & mask; // get a LSB from low
		words[i] = words[i] >> amount | temp; // set as amount MSB in high
	}

	words[len-1] = words[len-1] >> amount; // shift the low word
}

// Used in calculateTables
void Fingerprinter::multiT8(uint *words) {
	int needXOR;
	short i, j;
	for(i=0; i<8; i++){
		// XOR is needed only when the MSB = 1
		needXOR = ((words[wCount-1] & 0x80000000) == 0x80000000);
		// left shift 1 bit
		longLeftShift(words, wCount, 1);
		if ( needXOR ){
			for(j=wCount-1; j>=0; j--){
				words[j] = words[j] ^ P[j];
			}
		}
	}
}

// Used in calculateTables / fp4 or something
void Fingerprinter::byteShift(uint *words, short len, short value) {
	short i;
	words[len - 1] = value << 24;
	for(i = len - 2; i >= 0; i--){
		words[i] = 0x00000000;
	}
}

// Add a character to the fingerprint
uint* Fingerprinter::fp1(uint *current, char add) {
	uint W;
	int needXOR;
	short i, j;

	// add the character one bit at a time
	for ( i = 0; i < 8; i++ ) {
		W = add & 0x80; // retrieve the MSB of s
		add = add << 1; // shift for next round
		
		// get the MSB of the fingerprint
		needXOR = ( (current[wCount-1] & 0x80000000) == 0x80000000 );

		// Shift the fingerprint left by 1 bit
		longLeftShift(current, wCount, 1);

		// if we just pushed off a 1, xor with P
		if ( needXOR ){
			for( j = wCount - 1; j >= 0; j--) {
				current[j] = current[j] ^ P[j];
			}
		}

		// add on the new bit
		current[0] = current[0] ^ (W >> 7);
	}
	return current;
}

// Use the tables (TA, TB, etc) to add 4 bits at once. Copied from code
uint* Fingerprinter::fp4(uint *current, short s1, short s2, short s3, short s4) {
	uint W = 0;
	short j;
	unsigned short f1, f2, f3, f4;
	W = (((W | s1) << 8 | s2 ) << 8 | s3 ) << 8 | s4;
	f1 = (current[wCount - 1] >> 24 ) & 0xff;
	f2 = (current[wCount - 1] >> 16 ) & 0xff;
	f3 = (current[wCount - 1] >> 8 ) & 0xff;
	f4 = current[wCount - 1] & 0xff;
	for( j = wCount - 1; j > 0; j--) {
		current[j] = current[j-1] ^ TA[f1][j] ^ TB[f2][j] ^ TC[f3][j] ^ TD[f4][j];
	}
	current[0] = W ^ TA[f1][0] ^ TB[f2][0] ^ TC[f3][0] ^ TD[f4][0];
	return current;
}


double Fingerprinter::getArandom() {
	return rand()/(double)RAND_MAX;
}
