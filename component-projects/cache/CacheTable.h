#ifndef CACHE_TABLE
#define CACHE_TABLE

#include <vector>
#include <unordered_map>
#include <ctime>

typedef unsigned int uint;

struct CacheEntry {
	time_t timeCreated;
	time_t lastHit;
	uint *fP;
	unsigned long offset;
	char payLoad[1600];
	uint accessCount;
	uint missCount;
	uint hitCount;
	unsigned long long cumulativeGap;
	unsigned long long bytesSaved;
};



class CacheTable {
public:
	CacheTable();
	
	void addEntry(CacheEntry toAdd);
	CacheEntry getEntryAt(uint* fP);
	
	int getNumEntries();
	int getReplacementStrategy();
	std::unordered_map<uint*, CacheEntry> getEntries();
private:
	int numEntries;
	int replacementStrategy; // 1) LRU 2) Custom
	std::unordered_map<uint*, CacheEntry> *entries;
};

#endif
