CXX = g++
CPPFLAGS = -Isrc -DUSE_COLOR -ggdb
CXXFLAGS = -Wall -W -pedantic -std=gnu++11 -O1
LIBFLAGS = -Llib -lmcpdis # -lcrypto -lpthread
TARGETS = lib/libmcpdis.a bin/dis
LIBCC = src/mcpdis.cc src/term.cc src/pic12f.cc src/function.cc src/predicate.cc src/rule.cc src/range.cc src/grammar.cc
LIBHH = src/mcpdis.hh src/operators.hh src/ansicolor.hh

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

src/dis.o: src/dis.cc src/mcpdis.hh

lib/libmcpdis.a: $(LIBCC:.cc=.o)
	if [ ! -d lib ]; then mkdir -vp lib; fi
	ar crfv $@ $^ 

bin/dis: lib/libmcpdis.a src/dis.o
	if [ ! -d bin ]; then mkdir -vp bin; fi
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBFLAGS)
