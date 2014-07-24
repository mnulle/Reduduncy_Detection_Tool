/***************************************************************
 * File: CacheTable.h
 * Author: Matt Nulle
 * This header declares the CacheTable class with all its
 * variables and functions
 **************************************************************/

#ifndef CACHE_TABLE
#define CACHE_TABLE

#include <vector>
#include <fstream>
#include <iostream>
#include <sys/time.h>
//#include "runningxor.h" // simple fingerprinter
#include "rabin.h"	  // better fingerprinter
#include "EntryStats.h"

// typedef for brevity
typedef unsigned int uint;

// CacheEntry struct
struct CacheEntry {
	timeval timeCreated;	// time the entry is created
	timeval lastHit;	// time of the last access
	uint *fP;		// fingerprint of entry
	int fPlen;		// length of fingerprint
	unsigned long offset;	// offset in payload where fingerprint begins
	char payload[1600];	// payload of packet
	long expiryTime;	// time when packet can be removed from cache
	EntryStats *stats;	// Statistic gatherer
};

/**
 * Comparison operator for CacheEntry
 * args: 
 *   const CacheEntry& first: first CacheEntry to compare
 *   const CacheEntry& second: second CacheEntry to compare
 * return:
 *   true if effectively equal, false if not
 */
bool operator==(const CacheEntry& first, const CacheEntry& second);
/**
 * Comparison operator for CacheEntry
 * args: 
 *   const CacheEntry& first: first CacheEntry to compare
 *   const CacheEntry& second: second CacheEntry to compare
 * return:
 *   false if effectively equal, true if not
 */
bool operator!=(const CacheEntry& first, const CacheEntry& second);
/**
 * Function to make a CacheEntry given certain information
 * args: 
 *   uint* fp: fingerprint
 *   int fPlen: length of fingerprint in words
 *   char payload[1600]: payload fingerprint is from
 *   unsigned long offset: offset of fingerprint in payload
 *   uint expiry: amount of time before it expires
 * return:
 *   CacheEntry created with arguments provided
 */
CacheEntry makeEntry(uint* fp, int fPlen, char payload[1600], unsigned long offset, uint expiry);

// CacheTable class
class CacheTable {
public:
	/**
	 * Construct the cache table empty
	 */
	CacheTable();

	/**
	 * Free all memory allocated and controlled by cache table
	 */
	~CacheTable();

	/**
	 * Add an entry to the cache, depending on availability of its spot
	 * args: 
	 *   CacheEntry toAdd: CacheEntry to add to the table
	 */	
	void addEntry(CacheEntry toAdd);

	/**
	 * Get the entry that's in the spot fP hashes to
	 * args: 
	 *   uint* fP: fingerprint to determine location in table
	 *   int len: length of fingerprint in words
	 * return:
	 *   CacheEntry that is at the spot, possibly empty if never filled
	 */	
	CacheEntry getEntry(uint* fP, int len);

	/**
	 * Determines if the entry in fP's spot matches the
	 * the payload starting at offset, and if so, returns
	 * the position of the last character matched
	 * args: 
	 *   uint* fP: fingerprint to determine location in table
	 *   int fPlen: length of fingerprint in words
	 *   char payload[1600]: payload to compare to
	 *   int offset: offset in payload to compare at
	 * return:
	 *   -1 if no match, else index of last character matched
	 */	
	int matchAt(uint* fP, int fPlen, char payload[1600], int offset);

	/**
	 * Hashes the fingerprint using the Jenkin's hash (jfuncs.h)
	 * args: 
	 *   uint* fP: fingerprint to hash
	 *   int len: length of fingerprint
	 * return:
	 *   Hashed value
	 */	
	unsigned int hashFP(uint* fP, int len);

	/**
	 * Fills the table with entries spread out from this payload
	 * doesn't waste time looking for a match
	 * args: 
	 *   char paylaod[1600]: payload to chop up and enter
	 *   Fingerprinter *fper: fingerprinter to use to fingerprint payload
	 */	
	void bootstrapWith(char payload[1600], Fingerprinter *fper);

	/**
	 * Gets the number of entries entered in the table
	 * return:
	 *   The number of entries in the table
	 */	
	int getNumEntries();

	/**
	 * Get the hash table in vector form
	 * return:
	 *   The vector of entries
	 */	
	std::vector<CacheEntry>* getEntries();

	/**
	 * Empties out the cache and refills with default entries
	 */	
	void emptyCache();

	/**
	 * Set the length of time before entries can be removed
	 * args: 
	 *   uint time: length of time before entries can be removed
	 */	
	void setExpiry(uint time);

	/**
	 * Write the entry in csv form to the log/
	 * args:
	 *   CacheEntry entry: the entry to write to the log
	 */
	void writeEntryToLog(CacheEntry entry, char *filename=NULL);

	/**
	 * Write all the entries in the cache to the log file
	 */
	void writeAllEntriesToLog(int final);

	/**
	 * Write a message to the log
	 */
	void writeMessageToLog(char* message);

	/**
	 * Adds cache entries from a file
	 * args:
	 *   char* filename: file to read entries from
	 */
	void loadEntries(char* filename);

private:
	timeval splitTimeVal(std::string input);

	int numEntries; // Number of entries in the table now
	int numBuckets; // Number of available places for entries
	uint expiryTime; // Length of time before entries can be removed
	std::vector<CacheEntry> *entries; // vector of all buckets

	std::ofstream logfile; // File for logging entries
};

#endif
