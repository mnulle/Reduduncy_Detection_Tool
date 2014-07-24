/***************************************************************
 * File: analyse-file.cpp
 * Author: Matt Nulle
 * This file is a simple program to analyse a given file with
 * a given expiry time
 **************************************************************/

#include <iostream>
#include <unistd.h>
#include "CacheController.h"
#include "util.h"
#include <stdlib.h>

/**
 * args: 
 *   char* filename: filename to analyse
 *   int expiry: length of time before a cache entry can be removed
 * return:
 *   nothing. Prints the bytes saved to stdout
 */
void runFile(char* filename, int expiry) {
	// Create P
	uint *P = (uint *)malloc(2*sizeof(uint));
	for(int j = 0; j < 2; j++) P[j] = 0;
		P[0] = 3;
	// Create controller using p to analyse filename
	CacheController controller(filename, P);
	controller.setExpiry(expiry); // Set expiry

	// Process the file
	controller.processAllPacketsFromFile();

	// Print the bytes saved by the cache
	printf("%d\n", controller.getBytesSaved());
}

// Process arguments and call function to run the file
int main(int argc, char** argv) {
	int expiryTime;
	char* filename;
	// Check if expiry time is given
	if(argc < 2)
		expiryTime = 120;
	else
		expiryTime = atoi(argv[1]);

	// Check if filename is provided
	if(argc < 3)
		filename = (char*)"pcaps/Refreshing.pcapng";
	else
		filename = argv[2];

	// Print the expiry time, first half of output
	printf("%d,", expiryTime);
	
	// Run the file
	runFile(filename, expiryTime);
	//printf("Whooooooo, more commands!");
	return 0;
}
