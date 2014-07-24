#include <iostream>
#include <unistd.h>
#include "CacheTable.h"
#include "rabin.h"
#include "polynomial.h"
#include "PacketGrabber.h"
#include "CacheController.h"
#include "util.h"

using namespace std;

// Tests: Able to compile and run the program with libraries that are included.
int basic_output() {
	cout << "This is a series of tests to make sure that CacheTable and its\nassociated code is working properly.\n";
	return 0;
}

// Tests: Constructor of CacheTable
int create_cache() {
	CacheTable table;
	cout << "Loading entries\n";
	table.loadEntries("cache-final-state-log.txt");
	cout << "Done loading entries\n";
	return 0;
}

// Tests: Able to get the current time and construct a CacheEntry using it
int create_entry() {
	timeval tp;
	gettimeofday(&tp, 0);
	uint *fP = (uint *)malloc(sizeof(uint)); //allocate memory
	fP[0] = 0;
	CacheEntry entry = {
		tp,	// timeval timeCreated;
		tp,	// timeval lastHit;
		fP,	// uint *fP;
		1,	// int fPlen;
		0,	// unsigned long offset;
		"",	// char payLoad[1600];
		0,	// uint accessCount;
		0,	// uint missCount;
		0,	// uint hitCount;
		0,	// unsigned long long cumulativeGap;
		0	// unsigned long long bytesSaved;
	};
	free(fP);
	return entry.bytesSaved;
}
// Tests: Able to get the current time and add a CacheEntry to the cache using it
int add_entry() {
	timeval tp;
	gettimeofday(&tp, 0);
	uint *fP = (uint *)malloc(2*sizeof(uint)); //allocate memory
	fP[0] = 0;
	fP[1] = 0;
	CacheEntry entry = {
		tp,	// timeval timeCreated;
		tp,	// timeval lastHit;
		fP,	// uint *fP;
		2,	// int fPlen;
		0,	// unsigned long offset;
		"",	// char payLoad[1600];
		0,	// uint accessCount;
		0,	// uint missCount;
		0,	// uint hitCount;
		0,	// unsigned long long cumulativeGap;
		0	// unsigned long long bytesSaved;
	};
	CacheTable table;
	table.addEntry(entry);
//	free(fP);
	return 0;
}

// Tests: ability to create, add, and retrieve a cache entry. Editing entry created does not change entry in cache, except payload
int add_and_retrieve() {
	timeval tp;
	gettimeofday(&tp, 0);
	uint *fP = (uint *)malloc(sizeof(uint)); //allocate memory
	fP[0] = 2389;
	CacheEntry entry = {
		tp,	// timeval timeCreated;
		tp,	// timeval lastHit;
		fP,	// uint *fP;
		1,	// int fPlen;
		0,	// unsigned long offset;
		"",	// char payLoad[1600];
		0,	// uint accessCount;
		0,	// uint missCount;
		0,	// uint hitCount;
		0,	// unsigned long long cumulativeGap;
		0	// unsigned long long bytesSaved;
	};
	CacheTable table;
	table.addEntry(entry);
	CacheEntry retrieved;
	if((retrieved = table.getEntry(fP, 1)) != entry)  {
		printf("add_and_retrieve: Expected entries to be the same\n\tFirst\t\tSecond\n");
		printf("fP[0]\t%d\t\t%d\n", retrieved.fP[0], entry.fP[0]);
		printf("offset\t%lu\t\t%lu\n", retrieved.offset, entry.offset);
		printf("payload\t%s\t%s\n", retrieved.payload, entry.payload);
		return 1;
	}

	entry.offset = 10;
	if((retrieved = table.getEntry(fP, 1)) == entry)  {
		printf("add_and_retrieve: Expected entries to be different\n");
		return 2;
	}
	else entry.offset = table.getEntry(fP, 1).offset;

	entry.fP = (uint*) 92309239;
	if((retrieved = table.getEntry(fP, 1)) == entry)  {
		printf("add_and_retrieve: Expected entries to be different\n");
		return 3;
	}
	else entry.fP = table.getEntry(fP, 1).fP;
//	free(fP);
	return 0;	
}

// Tests the CacheTable matchAt function
int cache_matchAt() {
	timeval tp;
	gettimeofday(&tp, 0);
	uint *fP = (uint *)malloc(sizeof(uint)); //allocate memory
	fP[0] = 2389;
	CacheEntry entry = makeEntry(fP, 1, (char*) "abcdefghijklmnopqrstuvwxyz", 0, 120);
	CacheTable table;
	table.addEntry(entry);
	int matchTo = table.matchAt(fP, 1, (char* )"abcdefghijklmmmmmmmuvwxyz", 0);
	if(matchTo != 12) {
		printf("Last match should be 12\nLast match is        %d\n", matchTo);
		return matchTo;
	}
	return 0;
}

// Tests the different constructors of the Fingerprinter class
int fingerprinter_constructors() {
	// Default constructor
	Fingerprinter fper;

	// Constructor with size of p and window given
	Fingerprinter fper2(32, 8);
	
	// Constructor with size, window, and P supplied
	int k = 128;
	uint *P = (uint *)malloc(k/32*sizeof(uint));
	for(int j = 0; j < k / 32 - 1; j++) P[j] = 0;
	P[k / 32 - 1] = 3;
	Fingerprinter fper3(k, 16, P);

	return 0;
}

// Tests the fingerprintAll function
int fingerprinter_fingerprintAll() {
	int k = 64;
	int result = 0;
	uint *P = (uint *)malloc(k/32*sizeof(uint));
	uint *P2 = (uint *)malloc(k/32*sizeof(uint));
	for(int j = 0; j < k / 32; j++) P[j] = 0;
	P[0] = 3;
	for(int j = 0; j < k / 32; j++) P2[j] = 0;
	P2[0] = 3;

	Fingerprinter fper(k, 8, P);
	uint* fp;
	fp = fper.fingerprintAll((char*)"abcdabcd");
	for(int j = k / 32 - 1; j >= 0; j--)
		if(fp[j] != 0x61626364)
			result++;
	if(result) {
		cout << "  Fingerprint should be 61626364 61626364\n";
		cout << "  Fingerprint is        ";
		for(int j = k / 32 - 1; j >= 0; j--) 
			printf("%08X ", fp[j]);
		cout << endl;
	}
	free(fp);

	Fingerprinter fper2(k, 16, P2);
	fp = fper2.fingerprintAll((char*)"abcdabcdabcdabcd");
	for(int j = k / 32 - 1; j >= 0; j--)
		if(fp[j] != 0xC2C4C6C8)
			result++;

	if(result) {
		cout << "  Fingerprint should be C2C4C6C8 C2C4C6C8\n";
		cout << "  Fingerprint is        ";
		for(int j = k / 32 - 1; j >= 0; j--)
			printf("%08X ", fp[j]);
		cout << endl;
	}
	free(fp);	
	return result;
}

// Tests the fpn function
int fingerprinter_fpn() {
	int k = 64;
	int result = 0;
	uint *P = (uint *)malloc(k/32*sizeof(uint));
	uint *P2 = (uint *)malloc(k/32*sizeof(uint));
	for(int j = 0; j < k / 32; j++) P[j] = 0;
	P[0] = 3;
	for(int j = 0; j < k / 32; j++) P2[j] = 0;
	P2[0] = 3;

	Fingerprinter fper(k, 8, P);
	uint* fp;
	fp = fper.fpn((char*)"abcdabcdabcd", 0, 8);
	for(int j = k / 32 - 1; j >= 0; j--)
		if(fp[j] != 0x61626364)
			result++;
	if(result) {
		cout << "  Fingerprint should be 61626364 61626364\n";
		cout << "  Fingerprint is        ";
		for(int j = k / 32 - 1; j >= 0; j--) 
			printf("%08X ", fp[j]);
		cout << endl;
	}
	free(fp);

	Fingerprinter fper2(k, 16, P2);
	fp = fper2.fpn((char*)"abcdabcdabcdabcd", 0, 16);
	for(int j = k / 32 - 1; j >= 0; j--)
		if(fp[j] != 0xC2C4C6C8)
			result++;

	if(result) {
		cout << "  Fingerprint should be C2C4C6C8 C2C4C6C8\n";
		cout << "  Fingerprint is        ";
		for(int j = k / 32 - 1; j >= 0; j--)
			printf("%08X ", fp[j]);
		cout << endl;
	}
	free(fp);

	fp = fper2.fpn((char*)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 0, 16);
	for(int j = k / 32 - 1; j >= 0; j--)
		if(fp[j] != 0xC2C2C2C2)
			result++;
	if(result) {
		cout << "  Fingerprint should be C2C2C2C2 C2C2C2C2\n";
		cout << "  Fingerprint is        ";
		for(int j = k / 32 - 1; j >= 0; j--)
			printf("%08X ", fp[j]);
		cout << endl;
	}
	free(fp);
	

	return result;
}

// Tests the fp1shift function
int fingerprinter_fp1shift() {
	int k = 64;
	int result = 0;
	uint *P = (uint *)malloc(k/32*sizeof(uint));
	for(int j = 0; j < k / 32; j++) P[j] = 0;
	P[0] = 3;

	Fingerprinter fper(k, 8, P);
	uint* fp;
	
	fp = fper.fpn((char*)"abcdabcdabcdabcd", 0, 8);
	fp = fper.fp1shift(fp, 'a', 'a');

	for(int j = k / 32 - 1; j >= 0; j--)
		if(fp[j] != 0x62636461)
			result++;
	if(result) {
		cout << "  Fingerprint should be 62636461 62636461\n";
		cout << "  Fingerprint is        ";
		for(int j = k / 32 - 1; j >= 0; j--) {
			printf("%08X ", fp[j]);
		}
		cout << endl;
	}
	free(fp);

	uint *P2 = (uint *)malloc(k/32*sizeof(uint));
	for(int j = 0; j < k / 32; j++) P2[j] = 0;
	P2[0] = 3;


	Fingerprinter fper2(k, 16, P2);
	fp = fper2.fpn((char*)"abcdabcdabcdabcd", 0, 16);
	fp = fper2.fp1shift(fp, 'a', 'a');

	for(int j = k / 32 - 1; j >= 0; j--) {
		if(fp[j] != 0xC4C6C8C2)
			result++;
	}

	if(result) {
		cout << "  Fingerprint should be C4C6C8C2 C4C6C8C2\n";
		cout << "  Fingerprint is        ";
		for(int j = k / 32 - 1; j >= 0; j--) {
			printf("%08X ", fp[j]);
			if(fp[j] != 0xC4C6C8C2)
				result++;
		}
		cout << endl;
	}
	free(fp);

	return result;

}

// Tests the poly_mod function with a few use cases
int polynomial_first() {
	int retval = 0;
	uint *f, *p, *result;
	f = (uint *)malloc(1*sizeof(uint));
	p = (uint *)malloc(1*sizeof(uint));
	f[0] = 0x12345678;
	p[0] = 0x00000003;
	// f is the same size as p, should return f unchanged
	result = poly_mod(f, p, 1, 1);
	if(result[0] != 0x12345678) {
		printf("  Should be 12345678\n");
		printf("  It is     %08X\n", result[0]);
		retval = 1;
	}
	free(f);

	// f is twice as large, should modify
	f = (uint *)malloc(2*sizeof(uint));
	f[1] = 0x80000000;
	f[0] = 0x00000001;
	result = poly_mod(f, p, 2, 1);
	if(result[0] != 0x80000002) {
		printf("  Should be 80000002\n");
		printf("  It is     %08X\n", result[0]);
		retval += 2;
	}
	free(f);

	// f is still twice as large, other simple number
	f = (uint *)malloc(2*sizeof(uint));
	f[1] = 0x80000000;
	f[0] = 0x00000002;
	result = poly_mod(f, p, 2, 1);
	if(result[0] != 0x80000001) {
		printf("  Should be 80000001\n");
		printf("  It is     %08X\n", result[0]);
		retval += 4;
	}
	free(f);

	// f is real use case (t^31)
	f = (uint *)malloc(2*sizeof(uint));
	f[1] = 0x80000000;
	f[0] = 0x00000000;
	result = poly_mod(f, p, 2, 1);
	if(result[0] != 0x80000003) {
		printf("  Should be 80000003\n");
		printf("  It is     %08X\n", result[0]);	
		retval += 4;
	}
	free(f);

	free(p);
	return retval;
}

// Tests the packet grabber constructors
int tcp_constructor() {
	PacketGrabber pgrab((char* )"pcaps/short.cap");
	printf("  Should throw an error\n");
	PacketGrabber pgrab2((char* )"pcaps/TCP_doesnt_exist.cap");
	printf("\n");
	return 0;
}

// Tests the packet grabber "getNextPacket" function
int tcp_getNextPacket() {
	PacketGrabber pgrab((char* )"pcaps/short.cap");
	while(pgrab.getNextPacket())
		;
	return 0;
}

// Tests the packet grabber "openFile" function
int tcp_openFile() {
	PacketGrabber pgrab((char* )"pcaps/short.cap");
	pgrab.openFile((char*)"pcaps/TCP_example.cap");
	return 0;
}

// Tests the packet grabber "getCurrentPayload" function
int tcp_getCurrentPayload() {
	int fails = 0;
	PacketGrabber pgrab((char* )"pcaps/short.cap");
	char* payload;
	while(pgrab.getNextPacket()) {
		payload = pgrab.getCurrentPayload();
		if(payload[0] != '\0') {
			if(strncmp(payload, "GET /images/layout/logo.png HTTP/1.1", 83-47)) {
				printf("%s\n", payload);
				printf("Differs at %d\n", strncmp(payload, "GET /images/layout/logo.png HTTP/1.1", 83-47 -1));
				fails++;
			}
		}
	}
	return fails;
}

// Tests the CacheController constructors don't cause errors
int controller_constructors() {
	CacheController controller;
	CacheController cont2((char*) "pcaps/TCP_example.cap");
	return 0;
}

// Tests that cache controller readPacketsFromFile doesn't cause errors
int controller_readPacketsFromFile() {
	CacheController controller;
	controller.readPacketsFromFile((char*)"pcaps/TCP_example.cap");
	return 0;
}

// Tests the cache controller processNextPacket function
int controller_processNextPacket() {
	CacheController controller;
	int bytes = controller.processNextPacket();
	if(bytes != -1)
		return -4;
	bytes = controller.processNextPacket();
	if(bytes != -1)
		return -2;
	bytes = controller.processNextPacket();
	if(bytes != -1)
		return -3;
	bytes = controller.processNextPacket();
	if(bytes != 12)
		return bytes;
	return 0;
}

// Tests the controller processAll function
int controller_processAll() {
	int packets;
	uint *P = (uint *)malloc(2*sizeof(uint));
	for(int j = 0; j < 2; j++) P[j] = 0;
	P[0] = 3;
	CacheController controller(P);
	packets = controller.processAllPacketsFromFile();
	if(packets != 4) {
		printf("Should have processed 4 packets\n");
		printf("Did process           %d\n", packets);
		return 1;
	}

	controller.readPacketsFromFile((char*)"pcaps/test.cap");
	packets = controller.processAllPacketsFromFile();
	if(packets != 5) {
		printf("Should have processed 5 packets\n");
		printf("Did process           %d\n", packets);
		return 2;
	}
	if(controller.getBytesSaved() != 16) {
		printf("Should have saved 16 bytes\n");
		printf("Really saved      %d\n", controller.getBytesSaved());
		return 3;
	}
	controller.readPacketsFromFile((char*)"pcaps/test.cap");
	packets = controller.processAllPacketsFromFile();
	if(packets != 5) {
		printf("Should have processed 5 packets\n");
		printf("Did process           %d\n", packets);
		return 4;
	}
	if(controller.getBytesSaved() != 96) {
		printf("Should have saved 96 bytes\n");
		printf("Really saved      %d\n", controller.getBytesSaved());
		return 5;
	}
	return 0;
}

// Continues to test the controller processAll function
int controller_secondAll() {
	// TODO: Create more test files, and go to town on all the edge cases I can think of
	//	 Ideally, find ways to break it without making too large of packets/files.
	//	 All behavior should, theoretically, be observable with packets < 64 bytes
	// 	 And files <= 3 packets
	int packets;
	uint *P = (uint *)malloc(2*sizeof(uint));
	for(int j = 0; j < 2; j++) P[j] = 0;
	P[0] = 3;
	CacheController controller((char*)"pcaps/test2.cap", P);
	packets = controller.processAllPacketsFromFile();
	if(packets != 8) {
		printf("Should have processed 8 packets\n");
		printf("Did process           %d\n", packets);
		return 1;
	}
	if(controller.getBytesSaved() != 218) {
		printf("Should have saved 210 bytes\n");
		printf("Really saved      %d\n", controller.getBytesSaved());
		return 2;
	}
	controller.readPacketsFromFile((char*)"pcaps/test3.cap");
	controller.emptyCache();
	packets = controller.processAllPacketsFromFile();
	if(packets != 3) {
		printf("Should have processed 3 packets\n");
		printf("Did process           %d\n", packets);
		return 3;
	}
	if(controller.getBytesSaved() != 32) {
		printf("Should have saved 32 bytes\n");
		printf("Really saved      %d\n", controller.getBytesSaved());
		return 4;
	}

	return 0;
}

// Tests the util.h printTCP function. Visual inspection required
int util_printTCP() {
	char *s = (char*) "abcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwx\
abcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwx\
abcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwx";
	printTCPpayload(s);
	return 0;
}

// Runs a test function and prints data on whether it passed or not, and the error code of why not
int test(int (*pt2Func)(), int testNum) {
	int result;
	if((result = (*pt2Func)())) printf("Test %d\t: Fail (%d)\n", testNum, result);
	else printf("Test %d\t: Pass\n", testNum);
	return result;
}

// Tests the PacketGrabber functions
int basic_tcp_test() {
	int fails = 0, total = 0;
	cout << "----------------------------------------------\n  This is a series of tests for the basic\n  functionality of the packet grabber methods\n----------------------------------------------\n";
	if(test(&tcp_constructor, 1)) fails++;
	total++;
	//sleep(1);
	if(test(&tcp_getNextPacket, 2)) fails++;
	total++;
	//sleep(1);
	if(test(&tcp_getCurrentPayload, 3)) fails++;
	total++;
	//sleep(1);
	if(test(&tcp_openFile, 4)) fails++;
	total++;
	//sleep(1);
	cout << "----------------------------------------------\n  PacketGrabber Tests Completed\n  "<<total-fails<<"/"<<total<<" tests passed\n----------------------------------------------\n\n";	

	return fails;
}

// Tests the polynomial functions (i.e. poly_mod and helpers)
int basic_polynomial_test() {
	int fails = 0, total = 0;
	cout << "----------------------------------------------\n  This is a series of tests for the basic\n  functionality of the polynomial methods\n----------------------------------------------\n";
	if(test(&polynomial_first, 1)) fails++;
	total++;
	//sleep(1);
	cout << "----------------------------------------------\n  Polynomial Tests Completed\n  "<<total-fails<<"/"<<total<<" tests passed\n----------------------------------------------\n\n";	

	return fails;
}

// Tests the fingerprinter class
int basic_fingerprinter_test() {
	int fails = 0, total = 0;
	cout << "----------------------------------------------\n  This is a series of tests for the basic\n  functionality of the fingerprinter methods\n----------------------------------------------\n";

	if(test(&fingerprinter_constructors, 1)) fails++;
	total++;
	//sleep(1);
	if(test(&fingerprinter_fingerprintAll, 3)) fails++;
	total++;
	//sleep(1);
	if(test(&fingerprinter_fpn, 2)) fails++;
	total++;
	//sleep(1);
	if(test(&fingerprinter_fp1shift, 3)) fails++;
	total++;
	//sleep(1);

	cout << "----------------------------------------------\n  Fingerprinter Tests Completed\n  "<<total-fails<<"/"<<total<<" tests passed\n----------------------------------------------\n\n";

	return fails;
}

// Tests the cache
int basic_cache_test() {
	int fails = 0, total = 0;
	cout << "----------------------------------------------\n  This is a series of tests for the basic\n  functionality of the cache methods\n----------------------------------------------\n";
	if(test(&create_cache, 1)) fails++;
	total++;
	//sleep(1);
	if(test(&create_entry, 2)) fails++;
	total++;
	//sleep(1);
	if(test(&add_entry, 3)) fails++;
	total++;
	//sleep(1);
	if(test(&add_and_retrieve, 4)) fails++;
	total++;
	//sleep(1);
	if(test(&cache_matchAt, 5)) fails++;
	total++;
	//sleep(1);
	cout << "----------------------------------------------\n  Cache Tests Completed\n  "<<total-fails<<"/"<<total<<" tests passed\n----------------------------------------------\n\n";

	return fails;
}

int basic_controller_test() {
	int fails = 0, total = 0;
	cout << "----------------------------------------------\n  This is a series of tests for the basic\n  functionality of the controller methods\n----------------------------------------------\n";

	if(test(&controller_constructors, 1)) fails++;
	total++;
	//sleep(1);
	if(test(&controller_readPacketsFromFile, 2)) fails++;
	total++;
	//sleep(1);
	if(test(&controller_processNextPacket, 3)) fails++;
	total++;
	//sleep(1);
	if(test(&controller_processAll, 4)) fails++;
	total++;
	//sleep(1);
	if(test(&controller_secondAll, 5)) fails++;
	total++;
	//sleep(1);
	
	cout << "----------------------------------------------\n  Controller Tests Completed\n  "<<total-fails<<"/"<<total<<" tests passed\n----------------------------------------------\n\n";
	return fails;
}

void runFile(char* filename) {
	CacheController controller(filename);
	int packets = controller.processAllPacketsFromFile();
	printf("\n\n\n%s results:\n  Packets: %d\n  Bytes saved: %d\n", filename, packets, controller.getBytesSaved());
}

int main() {
	int testsFailed = 0;
	test(&basic_output, 1);
	//sleep(1);
	cout << endl;

	testsFailed += basic_cache_test();
	testsFailed += basic_polynomial_test();
	testsFailed += basic_fingerprinter_test();
	testsFailed += basic_tcp_test();
	testsFailed += basic_controller_test();

	//util_printTCP();

	cout << "Tests failed: " << testsFailed << endl;
	//runFile((char*)"pcaps/Refreshing.pcapng");
	return 0;
}
