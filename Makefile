CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++11 $(CXXOPTIMIZE)
USERID= 17344127
CLASSES= RoutingTable.o Datagram.o
INCL= /usr/local/include/boost_1_66_0

all: router server client

router server client: $(CLASSES)
	$(CXX) -I $(INCL) -o $@ $^ $(CXXFLAGS) $@.cpp /usr/local/include/boost_1_66_0/stage/lib/libboost_system.a

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM router server client *.tar.gz

tarball: clean
	tar -cvf $(USERID).tar.gz *
