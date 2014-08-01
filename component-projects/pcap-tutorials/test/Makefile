all: fileread streamread

fileread: fileread.c
	gcc fileread.c -lpcap -o fileread -Wall

streamread: streamread.c
	gcc streamread.c -lpcap -o streamread -Wall

clean:
	rm streamread fileread
