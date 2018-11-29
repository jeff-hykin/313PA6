# makefile

all: clean dataserver client

reqfifo.o: reqfifo.h reqfifo.cpp
	g++ -g -w -Wall -std=c++17 -c reqfifo.cpp

reqmq.o: reqmq.h reqmq.cpp
	g++ -g -w -Wall -std=c++17 -c reqmq.cpp

reqshm.o: reqshm.h reqshm.cpp
	g++ -g -w -Wall -std=c++17 -c reqshm.cpp

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -g -w -Wall -std=c++17 -c reqchannel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -g -w -Wall -std=c++17 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -Wall -std=c++17 -c Histogram.cpp

dataserver: dataserver.cpp reqchannel.o reqfifo.o reqmq.o reqshm.o
	g++ -g -w -Wall -std=c++17 -o dataserver dataserver.cpp reqchannel.o reqfifo.o reqmq.o reqshm.o -lpthread

client: client.cpp reqchannel.o BoundedBuffer.o Histogram.o
	g++ -g -w -Wall -std=c++17 -o client client.cpp reqchannel.o BoundedBuffer.o Histogram.o reqfifo.o reqmq.o reqshm.o -lpthread

clean:
	rm -rf *.o fifo* dataserver\ * client\ * temp_*
