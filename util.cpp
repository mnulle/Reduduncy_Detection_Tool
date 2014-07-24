#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sys/time.h>
#include <iostream>
#include <iomanip>

// Prints the whole length of the payload in a readable format for analysis
void printTCPpayload(char payload[1600]) {
	printTCPpayload(payload, strlen(payload)); // Pass the length of the string to the workhorse
}

// Prints len characters of the string
void printTCPpayload(char payload[1600], int len) {
	// Intro
	printf("Payload:\n");
	// Counts up how many bytes read for spacing
	int formatcount = 0;
	for(int i = 0; i < len; i++) {
		// Every byte is printed as 2 hex numbers with leading 0s
		printf("%02X ", (unsigned char)payload[i]);

		// Every eigth character, put a space in
		formatcount++;
		if(formatcount % 8 == 0)
			printf("  ");
		// Every five groups of 8, put a newline in
		if(formatcount == 40) {
			formatcount = 0;
			printf("\n");
		}
	}
	printf("\n");
}

// Write a message to the log file
void writeToLog(char* message, char* tag) {
	// Open the log file for appending
	std::ofstream logfile;
	logfile.open("log-file.txt", std::ios::out | std::ios::app);
	// make sure it opened
	if(logfile.is_open()) {
		// get the current time
		timeval tp;
		gettimeofday(&tp, 0);
		// record the entry
		// 		fill with 0s	10 digits for seconds			6 digits for millisecs
		logfile <<  std::setfill('0') << std::setw(10) << tp.tv_sec << "." << std::setw(6) << tp.tv_usec << ":\t" << message << "\t::" << tag << "\n";
		// close file
		logfile.close();
	}
}
