/***************************************************************
 * File: CacheTable.cpp
 * Author: Matt Nulle
 * This file implements the functions found in CacheTable.h
 **************************************************************/

#include "CacheTable.h"
#include "rabin.h"
#include <sys/time.h>
#include "jfuncs.h"
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "util.h"
#include <string>

CacheTable::CacheTable() {
	// init variables
	numEntries = 0;
	numBuckets = 100000;
	expiryTime = 120;
	entries = new std::vector<CacheEntry>(numBuckets);
	logfile.open("cache-log-file.txt", std::ios::out | std::ios::app);
}

CacheTable::~CacheTable() {
	writeMessageToLog((char*)"Deleting cache. Contents:\n<---Start Cache\n");
	writeAllEntriesToLog(1);
	writeMessageToLog((char*)"End Cache--->\n");
	// free fingerprints in entries
	for(uint i = 0; i < entries->size(); i++) {
		free((*entries)[i].fP);
	}
	// get rid of the table
	delete entries;
	if(logfile.is_open()) {
		logfile.close();
	}
}

void CacheTable::addEntry(CacheEntry toAdd) {
	// get the current time
	timeval tp;
	gettimeofday(&tp, 0);

	// hash the fingerprint and mod it with the number of buckets in the table
	int hash = hashFP(toAdd.fP, toAdd.fPlen) % numBuckets;

	// check if the bucket is available
	if(entries->at(hash).expiryTime < tp.tv_sec) {
		if(entries->at(hash).timeCreated.tv_sec != 0)
			writeEntryToLog(entries->at(hash));
		// free the fingerprint there (kinda surprised this (and destructor)
		// isn't causing problems with freeing unallocated memory)
		free(entries->at(hash).fP);

		// Old debugging prints, left just in case
		/*printf("\n\n\n-----------------------------------\n%s\n\n%lu\n%.8s\n", entries->at(hash).payload, entries->at(hash).offset, entries->at(hash).payload+entries->at(hash).offset);
		printf("%s\n\n%lu\n%.8s\n-------------------------------------\n\n\n", toAdd.payload, toAdd.offset, toAdd.payload+toAdd.offset);
		*/

		// Add the entry to the cache
		entries->at(hash) = toAdd;

		// if logging, append entry to file
		if(DEBUG_LOG) {
			char message[1000];
			char payloadChunk[17];
			char payloadStart[21];
			strncpy(payloadStart, toAdd.payload, 20); // get the beginning of the payload
			strncpy(payloadChunk, toAdd.payload + toAdd.offset, 16); // get the part of the payload being fingerprinted

			// null terminate both
			payloadChunk[16] = 0;
			payloadStart[20] = 0;

			// get all the info into the message string and write it to the log
			sprintf(message, "add: %s...%lu...%s:%X ", payloadStart, toAdd.offset, payloadChunk, toAdd.fP[0]);
			writeToLog(message, (char*)"TableAdd");
		}
	}
}

CacheEntry CacheTable::getEntry(uint* fP, int len) {
	// hash up the fingerprint and use it to get the entry
	int hash = hashFP(fP, len) % numBuckets;
	CacheEntry* entry = &entries->at(hash);

	// Make sure it's been actually entered
	if(entry->timeCreated.tv_sec != 0){
		timeval tp;
		gettimeofday(&tp, 0);
		// update hit information
		// TODO: more accurately update this stuff
		entry->lastHit = tp;
		entry->stats->Add_Stat(EntryStats::ACCESS, 1);
	}
	return *entry;
}

void CacheTable::emptyCache() {
	writeMessageToLog((char*)"Emptying cache. Current contents:\n<---Start Cache\n");
	writeAllEntriesToLog(0);
	writeMessageToLog((char*)"End Cache--->\n");
	entries->clear(); // clear out the cache
	entries->resize(numBuckets); // refill it with empty entries
}

int CacheTable::matchAt(uint* fP, int fPlen, char payload[1600], int offset) {
	// init last match to -1
	int lastMatch = -1;
	// get the entry at fP's spot
	CacheEntry entry = getEntry(fP, fPlen);

	// if there's an entry there
	if(entry.timeCreated.tv_sec != 0){
		//printf("Matches: ");
		// check to see how far the characters match
		for(uint i = offset; i < strlen(payload); i++) {
			// compare character by character
			if(payload[i] == entry.payload[i - offset + entry.offset]) {
				lastMatch = i;
			}
			else { // stop after first non-match
				//printf("\n");
				break;
			}
		}
		entry.stats->Add_Stat(EntryStats::BYTES_SAVED, lastMatch+1 - offset);
	}	

	// If there was a sufficiently big match and we're logging
	if(DEBUG_LOG && lastMatch + 1 - offset > 16) {
		char payloadStartOld[21];
		char payloadStartNew[21];
		char payloadChunkOld[17];
		char payloadChunkOldEnd[17];
		char payloadChunkNew[17];
		char message[1000];
		strncpy(payloadStartOld, entry.payload, 20); // get first part of payload in cache
		strncpy(payloadStartNew, payload, 20);	     // get first part of payload being compared
		strncpy(payloadChunkOld, entry.payload + entry.offset, 16); // get first part of section of payload in cache comparing
		strncpy(payloadChunkOldEnd, entry.payload + std::max(lastMatch - 15, 0), 16); // get end of section of payload in cache comparing
		strncpy(payloadChunkNew, payload + offset, 16);	// get first part of section in payload being compared

		// null terminate ALL the strings
		payloadStartOld[20] = 0;
		payloadStartNew[20] = 0;
		payloadChunkOld[16] = 0;
		payloadChunkOldEnd[16] = 0;
		payloadChunkNew[16] = 0;

		// Format it all into a single string, and log it
		sprintf(message, "old: %s...%lu...%s..%s\n\t\t\tnew: %s...%d...%s:%d", payloadStartOld, entry.offset, payloadChunkOld, payloadChunkOldEnd, payloadStartNew, offset, payloadChunkNew, lastMatch + 1 - offset );
		writeToLog(message, (char*)"TableMatch");

		// debugging prints, kept just in case
		//printf("Saved %d bytes!! Beginning at offset %d!\n", lastMatch+1, offset);
		//printTCPpayload(payload);
	}
	return lastMatch;
}

unsigned int CacheTable::hashFP(uint* fP, int len) {
	// calls hashword from jfuncs
	return hashword((uint32_t*)fP, len, 12345678);	
}

void CacheTable::bootstrapWith(char payload[1600], Fingerprinter* fper) {
	// set number of characters to jump. This is half hard-coded in
	// TODO: make the shifting in a for loop based on this variable
	uint bootstrapJump = 8;

	// get some variables set up for the process
	uint len = strlen(payload);		// length of packet
	uint window = fper->getWindow();	// window of bytes to process
	uint* temp;
	timeval tp;				// current time
	gettimeofday(&tp, 0);
	uint* fp = fper->fpn(payload, 0, window); // first fingerprint

	// make the first entry and add it
	CacheEntry entry = makeEntry(fp, fper->getWCount(), payload, 0, expiryTime);
	addEntry(entry);

	// Walk through the packet and insert chunks into the table
	for(int i = bootstrapJump; i <= (int)(len - window); i += bootstrapJump) {
		// create a new fingerprint for the new entry
		temp = (uint*)malloc(fper->getWCount()*sizeof(uint));
		memcpy(temp, fp, fper->getWCount()*sizeof(uint));
		fp = temp;

		// Now we can shift
		fper->fp1shift(fp, payload[i-8+window], payload[i-8]);
		fper->fp1shift(fp, payload[i-7+window], payload[i-7]);
		fper->fp1shift(fp, payload[i-6+window], payload[i-6]);
		fper->fp1shift(fp, payload[i-5+window], payload[i-5]);
		fper->fp1shift(fp, payload[i-4+window], payload[i-4]);
		fper->fp1shift(fp, payload[i-3+window], payload[i-3]);
		fper->fp1shift(fp, payload[i-2+window], payload[i-2]);
		fper->fp1shift(fp, payload[i-1+window], payload[i-1]);

		// make an entry out of the new fingerprint and add it		
		entry = makeEntry(fp, fper->getWCount(), payload, i, expiryTime);
		addEntry(entry);
	}
}

int CacheTable::getNumEntries() {
	return numEntries;
}

std::vector<CacheEntry>* CacheTable::getEntries() {
	return entries;
}

void CacheTable::setExpiry(uint time) {
	expiryTime = time;
}

bool operator==(const CacheEntry& first, const CacheEntry& second) {
	// compare fingerprints, offsets, and payloads
	if(first.fP == second.fP && first.offset == second.offset && !strcmp(first.payload, second.payload)) return 1;
	else return 0;
}

bool operator!=(const CacheEntry& first, const CacheEntry& second) {
	// compare fingerprints, offsets, and payloads
	if(first.fP == second.fP && first.offset == second.offset && !strcmp(first.payload, second.payload)) return 0;
	else return 1;
}

void CacheTable::writeAllEntriesToLog(int final) {
	if(final) {
		std::ofstream temp;
		temp.open("cache-final-state-log.txt", std::ofstream::trunc);
		temp.close();
	}
	for(std::vector<CacheEntry>::iterator it = entries->begin(); it != entries->end(); ++it) {
		if(it->timeCreated.tv_sec != 0) {
			writeEntryToLog(*it);
			if(final) writeEntryToLog(*it, (char*) "cache-final-state-log.txt");
		}
	}
}

void CacheTable::writeEntryToLog(CacheEntry entry, char* filename) {

	std::ofstream *logger;

	if(filename != NULL) {
		logger = new std::ofstream();
		logger->open(filename, std::ios::out | std::ios::app);
	} else {
		logger = &logfile;
	}


	// get the current time
	timeval tp;
	gettimeofday(&tp, 0);
	// record the entry
	// 		fill with 0s	10 digits for seconds			6 digits for millisecs
	*logger << std::setfill('0') << std::setw(10) << tp.tv_sec << "." << std::setw(6) << tp.tv_usec << ":\t";
	*logger << std::setfill('0') << std::setw(10) << entry.timeCreated.tv_sec << "." << std::setw(6) << entry.timeCreated.tv_usec << ",";
	*logger << std::setfill('0') << std::setw(10) << entry.lastHit.tv_sec << "." << std::setw(6) << entry.lastHit.tv_usec << ",";
	for(int i = entry.fPlen - 1; i >= 0; --i) {
		*logger << entry.fP[i];
		if(i != entry.fPlen-1)
			*logger << " ";
	}
	*logger << "," << entry.offset << "," << "," << entry.stats->Get_Stat(EntryStats::ACCESS) << "," << entry.stats->Get_Stat(EntryStats::MISS) << "," <<\
	entry.stats->Get_Stat(EntryStats::HIT) << "," << entry.expiryTime << "," << entry.stats->Get_Stat(EntryStats::BYTES_SAVED) << "," << strlen(entry.payload) << "," << entry.payload << "\n";
	if(filename != NULL)
		delete logger;
}

void CacheTable::writeMessageToLog(char* message) {
	// get the current time
	timeval tp;
	gettimeofday(&tp, 0);
	// record the entry
	// 		fill with 0s	10 digits for seconds			6 digits for millisecs
	logfile << std::setfill('0') << std::setw(10) << tp.tv_sec << "." << std::setw(6) << tp.tv_usec << ":\t" << message << "\n";		
}

void CacheTable::loadEntries(char* filename) {
	std::string input;
	std::ifstream file(filename);
	int i = 0, j = 0;

	timeval timeCreated;
	timeval lastHit;
	uint *fP;
	int fPlen;
	unsigned long offset;
	char payload[1600] = {0};
	uint accessCount;
	uint missCount;
	uint hitCount;
	long expiryTime;
	unsigned long long bytesSaved;
	
	while(file.good()) {
		std::getline ( file, input, '\t' );		

		std::getline ( file, input, ',' );
		timeCreated = splitTimeVal(input);

		std::getline ( file, input, ',' );
		lastHit = splitTimeVal(input);

		std::getline ( file, input, ',' );
		i = 0; j = input.find(' ');
		while(j != -1) { i++; j = input.find(' ', i + 1);}
		fPlen = i;
		fP = (uint *)malloc(fPlen*sizeof(uint));
		j = 0;
		for(i = fPlen - 1; i >= 0; --i) {
			fP[i] = (uint)atol(input.substr(j+1, input.find(' ', j+1)).c_str());
			j = input.find(' ', j+1);			
		}
		
		std::getline ( file, input, ',' );
		offset = (uint)atol(input.c_str());

		std::getline ( file, input, ',' );
		accessCount = (uint)atol(input.c_str());
		
		std::getline ( file, input, ',' );
		missCount = (uint)atol(input.c_str());

		std::getline ( file, input, ',' );
		hitCount = (uint)atol(input.c_str());

		std::getline ( file, input, ',' );
		expiryTime = atol(input.c_str());

		std::getline ( file, input, ',' );
		bytesSaved = (uint)atol(input.c_str());

		std::getline ( file, input, ',' );
		i = (uint)atol(input.c_str());

		file.read(payload, i);

		CacheEntry entry = {
			timeCreated,	// time the entry is created
			lastHit,	// time of the last access
			fP,		// fingerprint of entry
			fPlen,		// length of fingerprint
			offset,		// offset in payload where fingerprint begins
			"",		// payload of packet
			expiryTime,	// long expiryTime
			new EntryStats(), 
		};
		entry.stats->Set_Stat(EntryStats::ACCESS, accessCount);
		entry.stats->Set_Stat(EntryStats::MISS, missCount);
		entry.stats->Set_Stat(EntryStats::HIT, hitCount);
		entry.stats->Set_Stat(EntryStats::BYTES_SAVED, bytesSaved);
	
		strncpy(entry.payload, payload, 1600);
	}	
}

timeval CacheTable::splitTimeVal(std::string input) {
	int period = input.find('.');
	std::string seconds = input.substr(0, period);
	std::string milli = input.substr(period+1);
	timeval time = {atoi(seconds.c_str()), atoi(milli.c_str())};
	return time;
}

CacheEntry makeEntry(uint* fP, int fPlen, char payload[1600], unsigned long offset, uint expiry) {
	// get time of day
	timeval tp;
	gettimeofday(&tp, 0);

	// make the entry, leave payload blank and copy in after
	CacheEntry entry = {
		tp,	// timeval timeCreated;
		tp,	// timeval lastHit;
		fP,	// uint *fP;
		fPlen,	// int fPlen;
		offset,	// unsigned long offset;
		"",	// char payLoad[1600];
		expiry + tp.tv_sec,	// long expiryTime;
		new EntryStats()
	};

	// copy the payload in
	strncpy(entry.payload, payload, 1600);
	return entry;
}
