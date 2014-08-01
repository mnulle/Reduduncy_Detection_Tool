all: rabin

rabin: driver.o rabin.o polynomial.o jfuncs.o
	g++ jfuncs.o rabin.o driver.o polynomial.o -o rabin -Wall

jfuncs.o: jfuncs.c
	g++ -c jfuncs.c -Wall -g

rabin.o: rabin.cpp
	g++ -c rabin.cpp -Wall -g

polynomial.o: polynomial.c
	g++ -c polynomial.c -Wall -g

driver.o: driver.cpp rabin.cpp
	g++ -c driver.cpp -Wall -g

clean: 
	rm a.out rabin *.o *~ rabin-debug poly-test

debug: rabin-debug

rabin-debug: source.cpp
	g++ -g source.cpp polynomial.c -o rabin-debug -Wall
