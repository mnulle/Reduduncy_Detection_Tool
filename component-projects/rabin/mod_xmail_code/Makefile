all: rabin poly-test

rabin: source.o polynomial.o
	g++ source.o polynomial.o -o rabin -Wall

poly-test: polynomial.o poly-test.o
	g++ polynomial.o poly-test.o -o poly-test 

poly-test.o: poly-test.c
	g++ -c poly-test.c -g

polynomial.o: polynomial.c
	g++ -c polynomial.c -Wall -g

source.o: source.cpp
	g++ -c source.cpp -Wall

clean: 
	rm a.out rabin *.o *~ rabin-debug poly-test

debug: rabin-debug

rabin-debug: source.cpp
	g++ -g source.cpp polynomial.c -o rabin-debug -Wall
