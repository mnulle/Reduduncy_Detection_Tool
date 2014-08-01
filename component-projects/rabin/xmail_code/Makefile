all: rabin

rabin: source.o
	g++ source.o -o rabin -Wall

source.o: source.cpp
	g++ -c source.cpp -Wall

clean: 
	rm a.out rabin *.o *~
