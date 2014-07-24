all: cache-test analyse-file

cache-test: cache-test.o CacheTable.o CacheController.o jfuncs.o rabin.o polynomial.o PacketGrabber.o util.o Stats.o EntryStats.o
	g++ cache-test.o jfuncs.o rabin.o polynomial.o CacheTable.o CacheController.o PacketGrabber.o util.o Stats.o EntryStats.o -o cache-test -Wall -lpcap

analyse-file: analyse-file.o CacheTable.o CacheController.o jfuncs.o rabin.o polynomial.o PacketGrabber.o util.o Stats.o EntryStats.o
	g++ analyse-file.o CacheTable.o CacheController.o jfuncs.o rabin.o polynomial.o PacketGrabber.o util.o Stats.o EntryStats.o -o analyse-file -Wall -lpcap
#analyse-file: analyse-file.o CacheTable.o CacheController.o jfuncs.o runningxor.o polynomial.o PacketGrabber.o util.o 
#	g++ analyse-file.o CacheTable.o CacheController.o jfuncs.o runningxor.o polynomial.o PacketGrabber.o util.o -o analyse-file -Wall -lpcap

cache-test.o: cache-test.cpp
	g++ -c -g cache-test.cpp -Wall

analyse-file.o:	analyse-file.cpp
	g++ -c -g analyse-file.cpp -Wall

EntryStats.o: Stats.cc EntryStats.cc
	g++ -c -g EntryStats.cc -Wall

Stats.o: Stats.cc
	g++ -c -g Stats.cc -Wall

util.o: util.cpp
	g++ -c -g util.cpp -Wall

runningxor.o: runningxor.cpp
	g++ -c -g runningxor.cpp -Wall

PacketGrabber.o: PacketGrabber.cpp
	g++ -c -g PacketGrabber.cpp -Wall

CacheController.o: CacheController.cpp
	g++ -c -g CacheController.cpp -Wall

jfuncs.o: jfuncs.c
	g++ -c jfuncs.c -g

rabin.o: rabin.cpp
	g++ -c rabin.cpp -Wall -g

polynomial.o: polynomial.c
	g++ -c polynomial.c -Wall -g

CacheTable.o: CacheTable.cpp
	g++ -c -g CacheTable.cpp -Wall

clean:
	rm *.o cache-test analyse-file *~
