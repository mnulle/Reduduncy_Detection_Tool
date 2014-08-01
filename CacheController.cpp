/***************************************************************
 * File: polynomial.c
 * Author: Matt Nulle
 * This file implements the functions found in polynomial.h
 **************************************************************/

#include "CacheController.h"
#include "util.h"
#include "TableStats.h"
#include <stdio.h>

CacheController::CacheController():
	fper(64, 16),	// construct fingerprinter
	pgrab((char*)"pcaps/short.cap") { // construct packet grabber
	bytesSavedTotal = 0; // init to 0 bytes saved
	expiryTime = 120; // default to 2 minute expiration
	recordFlag = 0;

	writeToLog((char*)"Reading from pcaps/short.cap", (char*)"file"); // write to log
}

CacheController::CacheController(uint* P):
	fper(64, 16, P), // construct fingerprinter
	pgrab((char*)"pcaps/short.cap") { // construct packet grabber
	bytesSavedTotal = 0; // init to 0 bytes saved
	expiryTime = 120; // default to 2 minute expiration
	recordFlag = 0;

	writeToLog((char*)"Reading from pcaps/short.cap", (char*)"file"); // write to log
}

CacheController::CacheController(char *packetfile):
	fper(64, 16), // construct fingerprinter
	pgrab(packetfile) { // construct packet grabber
	bytesSavedTotal = 0; // init to 0 bytes saved
	expiryTime = 120; // default to 2 minute expiration
	recordFlag = 0;

	char message[1000]; // write to log
	sprintf(message, "Reading from %s", packetfile);
	writeToLog(message, (char*)"file");
}

CacheController::CacheController(char *packetfile, uint* P):
	fper(64, 16, P), // construct fingerprinter
	pgrab(packetfile) { // construct packet grabber
	bytesSavedTotal = 0; // init to 0 bytes saved
	expiryTime = 120; // default to 2 minute expiration
	recordFlag = 0;

	char message[1000]; // write to log
	sprintf(message, "Reading from %s", packetfile);
	writeToLog(message, (char*)"file");
}

CacheController::~CacheController() {
}

int CacheController::processNextPacket() {
	pgrab.getNextPacket(); // get the next packet
	if(strcmp(pgrab.getCurrentPayload(), "")) // if it's not empty, process it
		return processPacket(pgrab.getCurrentPayload(), pgrab.getPayloadCount());
	else
		return -1;
}

int CacheController::processAllPacketsFromFile() {
	int count = 0; // set count to 0
	while(pgrab.getNextPacket()) { // process each packet while there's more to process
		count++;
		if(strcmp(pgrab.getCurrentPayload(), "")) // process if it's not empty
			processPacket(pgrab.getCurrentPayload(), pgrab.getPayloadCount());
		if(pgrab.getPayloadCount() % 50 == 0 && !recordFlag) {
			cache.writeStatsToLog((char*)"stats.log");
			recordFlag = 1;
		}
		else if (pgrab.getPayloadCount() % 50 != 0)
			recordFlag = 0;
	}
	// return number of packets processed
	return count;
}

void CacheController::readPacketsFromFile(char* filename) {
	pgrab.openFile(filename); // open new file

	// write to log
	char message[1000]; 
	sprintf(message, "Reading from %s", filename);
	writeToLog(message, (char*)"file");
}

int CacheController::processPacket(char* payload, int count) {
	int bytesSaved = 0, i = 1;
	int payloadlen = strlen(payload);
	int endOfMatch = 0, window = fper.getWindow(), wCount = fper.getWCount();
	uint *fp, *temp;

	//printf("Deal with packet %d\n", pgrab.getPacketCount());

	if(count == 1) { // if the first packet, bootstrap with it
		cache.bootstrapWith(payload, &fper);
	}
	else {
		// start fingerprinting it, and checking for matches
		fp = fper.fpn(payload, 0, window);
		if((endOfMatch = cache.matchAt(fp, wCount, payload, 0)) >= window-1) {
			// Found 
			bytesSaved += endOfMatch + 1;

			// write to log
			if(DEBUG_LOG) {
				char message[1000];
				sprintf(message, "%d:%d::%d", count, 0, endOfMatch+1);
				writeToLog(message, (char*)"ControllerBytes");
			}

			// move up to the part of the packet after the match
			i = endOfMatch;
			if(i < payloadlen-window) {
				// these two could be combined using the 4 argument one
				// TODO: Combine
				free(fp); // free old fingerprint
				fp = fper.fpn(payload, i, window); // make a new one
			}
		}
		else {
			// No match
			// Add to cache and then keep looking
			cache.addEntry(makeEntry(fp, wCount, payload, 0, expiryTime));

			// Make a new fingerprint to keep searching with
			temp = (uint*)malloc(fper.getWCount()*sizeof(uint));
			memcpy(temp, fp, fper.getWCount()*sizeof(uint));
			fp = temp;
		}
		for(; i < payloadlen-window; i++) {
			fp = fper.fp1shift(fp, payload[i+window], payload[i]); // shift the fingerprint to the next character
			if((endOfMatch = cache.matchAt(fp, wCount, payload, i+1)) >= window-1 + i) { // check for match
				// Match 
				bytesSaved += endOfMatch - i;

				// Write to log
				if(DEBUG_LOG) {
					char message[1000];
					sprintf(message, "%d:%d::%d", count, i, endOfMatch - i);
					writeToLog(message, (char*)"ControllerBytes");
				}

				// move to section after match
				i = endOfMatch;
				if(i < payloadlen-window) {
					free(fp);
					fp = fper.fpn(payload, i, window);
				}
			}
			else {
				// No match
				// Add to cache and then keep looking
				cache.addEntry(makeEntry(fp, wCount, payload, i+1, expiryTime));

				temp = (uint*)malloc(fper.getWCount()*sizeof(uint));
				memcpy(temp, fp, fper.getWCount()*sizeof(uint));
				fp = temp;

			}
		}
		// free up the fingerprint memory
		free(fp);

	}
	bytesSavedTotal += bytesSaved; // add bytes saved from this packet to total
	cache.addStat(TableStats::BYTES_PROCESSED, payloadlen);
	cache.addStat(TableStats::PACKETS_PROCESSED, 1);
	return bytesSaved;
}

void CacheController::emptyCache() {
	// empty out the cache
	cache.emptyCache();
	// reset to 0 processed everywhere
	pgrab.resetCounts();
	// 0 bytes saved now
	bytesSavedTotal = 0;
}

void CacheController::setExpiry(long time) {
	expiryTime = time;
}

uint CacheController::getBytesSaved() {
	return bytesSavedTotal;
}

