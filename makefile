# makefile

all: clean dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -O3 -g -w -Wall -O1 -std=c++17 -c reqchannel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -O3 -g -w -Wall -O1 -std=c++17 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -O3 -g -w -Wall -O1 -std=c++17 -c Histogram.cpp


dataserver: dataserver.cpp reqchannel.o 
	g++ -O3 -g -w -Wall -O1 -std=c++17 -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o BoundedBuffer.o Histogram.o
	g++ -O3 -g -w -Wall -O1 -std=c++17 -o client client.cpp reqchannel.o BoundedBuffer.o Histogram.o -lpthread

clean:
	rm -rf *.o fifo* dataserver\ * client\ *
