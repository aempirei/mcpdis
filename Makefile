CXX = g++
CPPFLAGS = -Isrc -DUSE_COLOR
CXXFLAGS = -Wall -W -pedantic -std=gnu++11 -O3
LIBFLAGS = -Llib -lmcpdis -lyyy
TARGETS = lib/libmcpdis.a lib/libyyy.a bin/dis bin/either.demo bin/yyy.demo
LIBCC = src/mcpdis.cc src/term.cc src/pic12f.cc src/fn.cc src/predicate.cc src/range.cc src/grammar.cc
LIBHH = src/mcpdis.hh src/operators.hh src/ansicolor.hh
YYYCC = src/yyy/grammar.cc src/yyy/binding.cc src/yyy/function.cc src/yyy/predicate.cc src/yyy.cc
YYYHH = $(YYYCC:.cc=.hh)
TPLHH = src/yyy/choice.template.hh src/yyy/symbol.template.hh
# MAYBE = src/maybe.cc src/maybe.hh src/maybe.template.hh
# EITHER = src/either.cc src/either.hh src/either.template.hh

.PHONY: all clean test demo

all: $(TARGETS)

clean:
	rm -f $(TARGETS)
	rm -f src/*~ src/*.o src/yyy/*.o src/yyy/*~
	rm -f test.dis
	rm -rf bin lib

test: all
	./bin/dis -x le16 < test.bin > test.dis
	cat test.dis
	md5sum test.dis

demo: bin/yyy.demo bin/either.demo

src/dis.o: src/dis.cc src/mcpdis.hh

src/yyy.demo.o: src/yyy.demo.cc $(YYYHH) $(YYYCC) $(TPLHH)

src/either.demo.o: src/either.demo.cc $(EITHER) $(MAYBE)

lib/libmcpdis.a: $(LIBCC:.cc=.o)
	if [ ! -d lib ]; then mkdir -vp lib; fi
	ar crfv $@ $^ 

lib/libyyy.a: $(YYYCC:.cc=.o)
	if [ ! -d lib ]; then mkdir -vp lib; fi
	ar crfv $@ $^ 

bin/either.demo: src/either.demo.o
	if [ ! -d bin ]; then mkdir -vp bin; fi
	$(CXX) $(CXXFLAGS) -o $@ $^

bin/yyy.demo: lib/libyyy.a src/yyy.demo.o
	if [ ! -d bin ]; then mkdir -vp bin; fi
	$(CXX) $(CXXFLAGS) -o $@ $^ -Llib -lyyy

bin/dis: lib/libmcpdis.a src/dis.o
	if [ ! -d bin ]; then mkdir -vp bin; fi
	$(CXX) $(CXXFLAGS) -o $@ $^ -Llib -lmcpdis
