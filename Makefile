CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++11 $(CXXOPTIMIZE)
USERID= 17344127
CLASSES= 
INCL= boost_1_66_0

all: router

router: $(CLASSES)
	$(CXX) -I $(INCL) -o $@ $^ $(CXXFLAGS) $@.cpp

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM web-server web-client *.tar.gz

tarball: clean
	tar -cvf $(USERID).tar.gz *
