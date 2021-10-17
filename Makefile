all: start

shell.o: shell.h shell.cpp
	g++ -g -w -std=c++11 -c shell.cpp

parser.o: parser.h parser.cpp
	g++ -g -w -std=c++11 -c parser.cpp

start: start.cpp shell.o parser.o
	g++ -g -w -std=c++11 -o start start.cpp shell.o parser.o

clean:
	rm *.o start
