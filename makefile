# makefile

all: start

parser.o: parser.h parser.cpp
	g++ -g -w -std=c++11 -c parser.cpp

start: start.cpp parser.o
	g++ -g -w -std=c++11 -o start start.cpp parser.o

clean:
	rm *.o start
