CXX = g++
CPPFLAGS = -Isrc -DUSE_COLOR
CXXFLAGS = -Wall -W -pedantic -std=gnu++11 -O3
LIBFLAGS = -Llib -lmcpdis
TARGETS = lib/libmcpdis.a bin/dis bin/either.demo bin/yyy
LIBCC = src/mcpdis.cc src/term.cc src/pic12f.cc src/fn.cc src/predicate.cc src/range.cc src/grammar.cc
LIBHH = src/mcpdis.hh src/operators.hh src/ansicolor.hh
MAYBE = src/maybe.cc src/maybe.hh src/maybe.template.hh
EITHER = src/either.cc src/either.hh src/either.template.hh

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

src/yyy.o: src/yyy.cc src/yyy.hh $(EITHER) $(MAYBE)

src/either.demo.o: src/either.demo.cc $(EITHER) $(MAYBE)

lib/libmcpdis.a: $(LIBCC:.cc=.o)
	if [ ! -d lib ]; then mkdir -vp lib; fi
	ar crfv $@ $^ 

bin/either.demo: lib/libmcpdis.a src/either.demo.o
	if [ ! -d bin ]; then mkdir -vp bin; fi
	$(CXX) $(CXXFLAGS) -o $@ $^

bin/yyy: src/yyy.o
	if [ ! -d bin ]; then mkdir -vp bin; fi
	$(CXX) $(CXXFLAGS) -o $@ $^

bin/dis: lib/libmcpdis.a src/dis.o
	if [ ! -d bin ]; then mkdir -vp bin; fi
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBFLAGS)
