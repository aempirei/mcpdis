CXX = g++

CPPFLAGS = -Isrc
CXXFLAGS = -Wall -W -DUSE_COLOR -pedantic -std=gnu++11 -O0
LIBFLAGS = -Llib -lmcpdis -lyyy

TARGETS = lib/libmcpdis.a lib/libyyy.a bin/dis bin/yyy.demo

LIBCC = src/mcpdis.cc src/pic12f.cc
LIBHH = $(LIBCC:.cc=.hh)

YYYCC = src/yyy/grammar.cc src/yyy/binding.cc src/yyy/function.cc src/yyy/predicate.cc src/yyy.cc
YYYHH = $(YYYCC:.cc=.hh)

TPLHH = src/yyy/choice.template.hh src/yyy/symbol.template.hh src/yyy/type.template.hh

.PHONY: all clean test demo lib rebuild

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

########
# demo #
########

demo: bin/yyy.demo
	./bin/yyy.demo 111

src/yyy.demo.o: src/yyy.demo.cc $(TPLHH)

bin/yyy.demo: lib/libyyy.a src/yyy.demo.o
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^ -Llib -lyyy

#######
# lib #
#######

lib: lib/libyyy.a lib/libmcpdis.a

lib/libmcpdis.a: $(LIBCC:.cc=.o)
	mkdir -p lib
	ar crfv $@ $^

lib/libyyy.a: $(YYYCC:.cc=.o)
	mkdir -p lib
	ar crfv $@ $(YYYCC:.cc=.o)

#######
# dis #
#######

src/dis.o: src/dis.cc

bin/dis: lib/libmcpdis.a src/dis.o
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^ -Llib -lmcpdis -lyyy
