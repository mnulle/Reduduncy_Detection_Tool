/***************************************************************
 * File: CacheController.h
 * Author: Matt Nulle
 * This header declares the CacheController class
 **************************************************************/

#ifndef CACHE_CONTROLLER
#define CACHE_CONTROLLER

//#include "runningxor.h"
#include "rabin.h"
#include "CacheTable.h"
#include "PacketGrabber.h"

// The CacheController class
class CacheController {
public:
	// Constructors
	CacheController();
	// Supplied P for fingerprinter
	CacheController(unsigned int *P);
	// Supplied filename to process
	CacheController(char *packetfile);
	// Supplied P for fingerprinter and filename to process
	CacheController(char *packetfile, unsigned int *P);

	// Destructor
	~CacheController();

	// Open a file to read packets from
	void readPacketsFromFile(char* filename);

	// Process all the packets from the file
	int processAllPacketsFromFile();
	// Process just the next packet in the file
	int processNextPacket();
	// Process the given payload, given that it's the 'count' packet seen
	int processPacket(char* payload, int count);

	// Get the number of bytes saved
	uint getBytesSaved();

	// Empty out the cache
	void emptyCache();

	// Set the amount of time until entries can be replaced
	void setExpiry(long time);

private:
	// Fingerprinter to process packets
	Fingerprinter fper;
	// Cache
	CacheTable cache;
	// Class to get packets out of file
	PacketGrabber pgrab;
	// number of bytes saved so far
	uint bytesSavedTotal;
	// amount of time until entries can be replaced
	uint expiryTime;
};

#endif
