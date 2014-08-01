all: poly-test rabin

rabin: rabin.o rabin-test.o
	gcc rabin.o rabin-test.o -o rabin -Wall

rabin-test.o: rabin-test.c
	gcc -c rabin-test.c -Wall

rabin.o: rabin.c
	gcc -c rabin.c -Wall

poly-test: polynomial.o polynomial-test.o
	gcc polynomial.o polynomial-test.o -o poly-test -Wall

polynomial.o: polynomial.c
	gcc -c polynomial.c -Wall

polynomial-test.o: polynomial-test.c polynomial.h
	gcc -c polynomial-test.c -Wall

clean:
	rm *.o poly-test rabin *~
