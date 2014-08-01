#include "CacheTable.h"
#include <unordered_map>

CacheTable::CacheTable() {
	// TODO: use a constructor that allows you to pass it the jenkins hasher
	// Possibly, might need to use a more 'formal' array instead of int*, or do the hashing myself
	// 	Not sure how it can know how much of the array is actually in the array
	entries = new std::unordered_map<uint*, CacheEntry>(); 
	numEntries = 0;
}

void CacheTable::addEntry(CacheEntry toAdd) {
	entries->insert(std::make_pair<uint*, CacheEntry>((uint*)toAdd.fP, CacheEntry(toAdd)));
}

CacheEntry CacheTable::getEntryAt(uint* fP) {
	return entries->at(fP);
}


int CacheTable::getNumEntries() {
	return numEntries;
}

int CacheTable::getReplacementStrategy() {
	return replacementStrategy;
}
