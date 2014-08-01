all: cache-test

cache-test: cache-test.o CacheTable.o
	g++ cache-test.o CacheTable.o -o cache-test -Wall

cache-test.o: cache-test.cpp
	g++ -c -g cache-test.cpp -std=c++11 -Wall

CacheTable.o: CacheTable.cpp
	g++ -c -g CacheTable.cpp -std=c++11 -Wall

clean:
	rm *.o cache-test *~
