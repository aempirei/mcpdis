CXX = g++
CPPFLAGS = -Isrc -DUSE_COLOR -ggdb
CXXFLAGS = -Wall -W -pedantic -std=gnu++11 -O2 -ggdb
LIBFLAGS = -Llib -lmcpdis # -lcrypto -lpthread
TARGETS = lib/libmcpdis.a bin/dis

.PHONY: all clean test

all: $(TARGETS)

clean:
	rm -f src/*~ src/*.o $(TARGETS)
	rm -f test.dis
	rm -rf bin lib

test: all
	./bin/dis -x le16 < test.bin > test.dis
	cat test.dis
	md5sum test.dis

src/mcpdis.o: src/mcpdis.hh src/operators.hh src/ansicolor.hh src/mcpdis.cc src/term.cc src/pic12f.cc src/parser.cc src/function.cc

src/dis.o: src/dis.cc src/mcpdis.hh

lib/libmcpdis.a: src/mcpdis.o src/term.o src/pic12f.o src/parser.o src/function.o
	if [ ! -d lib ]; then mkdir -vp lib; fi
	ar crfv $@ $^ 

bin/dis: lib/libmcpdis.a src/dis.o
	if [ ! -d bin ]; then mkdir -vp bin; fi
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBFLAGS)
