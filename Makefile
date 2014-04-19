CXX = g++
CPPFLAGS = -Isrc
CXXFLAGS = -Wall -W -pedantic -std=gnu++11 -O3
CFLAGS = -Wall -W -pedantic -std=c99 -O3
LIBFLAGS = -Llib -lmcpdis -lcrypto -lpthread
TARGETS = lib/libmcpdis.a bin/dis

.PHONY: all clean test

all: $(TARGETS)

clean:
	rm -f src/*~ src/*.o $(TARGETS)
	rm -rf bin lib

test: all
	dd if=/dev/urandom of=/dev/stdout bs=35 count=1 2>/dev/null | ./bin/dis

src/mcpdis.o: src/mcpdis.cc src/mcpdis.hh

src/dis.o: src/dis.cc src/mcpdis.hh

lib/libmcpdis.a: src/mcpdis.o
	if [ ! -d lib ]; then mkdir -vp lib; fi
	ar crfv $@ $^ 

bin/dis: lib/libmcpdis.a src/dis.o
	if [ ! -d bin ]; then mkdir -vp bin; fi
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBFLAGS)
