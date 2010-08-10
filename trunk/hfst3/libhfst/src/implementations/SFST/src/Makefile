
# TYPE is either debug, prof, static, lib, Sun64bit or the empty string
TYPE = 

# try different definitions of SGILIB if hash_map is not found
#SGILIB=
SGILIB=-DSGI__gnu_cxx
#SGILIB=-DSGIext

# comment the following line if readline is not found
READLINE = -DREADLINE
LREADLINE = -lreadline -lhistory -lncurses

# data type used for the encoding of the symbols (char, short, or int)
# short is the default
#CDT=-DCODE_DATA_TYPE=short

CXX = g++
WARNING = -Wall -Wcast-qual -Wconversion -std=c++98 -Wno-deprecated -ansi

ifeq ($(TYPE),debug)
  DEBUG = -ggdb
  OPTIMIZATION = -O0
else
  DEBUG = -DNDEBUG
  ifeq ($(TYPE),prof)
    OPTIMIZATION = -O2
    PROFILE = -pg
  else
    OPTIMIZATION = -O3
    ifeq ($(TYPE),static)
      STATIC = -static
    else
      ifeq ($(TYPE),Sun64bit)
        LOPT = -L/usr/lib/64 -L/usr/local/lib/sparcv9 -R/usr/lib/64 -R/usr/local/lib/sparcv9
	CXX = g++64
	BITS= -mcpu=v9
      else
        ifeq ($(TYPE),lib)
          LIB = -fPIC
        endif
      endif
    endif
  endif
endif


CC = g++
CL = $(CC)
CFLAGS = $(DEBUG) $(PROFILE) $(LIB) $(OPTIMIZATION) $(WARNING) $(SGILIB) $(CDT) $(BITS) $(READLINE)
LDFLAGS = $(DEBUG) $(PROFILE) $(STATIC) $(LOPT)

CXXFLAGS = $(CFLAGS)
LXXFLAGS = $(LDFLAGS)

INSTALL = install
INSTALL_MANPAGES = install -m 644
INSTALL_DIR = install -d -m 755

DESTDIR =
PREFIX = /usr/local/

PROGRAMS = fst-compiler fst-infl fst-generate fst-print fst-compare \
	fst-compact fst-infl2 fst-lowmem fst-infl3 fst-lattice fst-match \
	fst-parse fst-parse2 fst-train fst-compiler-utf8 fst-text2bin fst-mor
ALLPROGRAMS = $(PROGRAMS)

ifeq ($(TYPE),lib)
TARGET = libsfst.tar.gz
else
TARGET = strip
endif


OBJ = basic.o alphabet.o fst.o operators.o determinise.o utf8.o

.PHONY: all clean realclean archive remake install strip

all: $(TARGET)

strip: $(ALLPROGRAMS)
	strip $(ALLPROGRAMS)

fst-api.zip: test.C alphabet.C alphabet.h basic.C basic.h compact.C \
	compact.h lowmem.C lowmem.h robust.C
	zip $@ $^

fst-compact: compact.o make-compact.o

fst-generate: generate.o

fst-match: compact.o

fst-train: compact.o

fst-compiler: $(OBJ) compact.o make-compact.o interface.o default-scanner.o

fst-compiler-utf8: $(OBJ) compact.o make-compact.o interface.o \
	utf8-scanner.o fst-compiler.o
	$(CXX) -o $@ $^ $(LDFLAGS)

fst-mor: $(OBJ) fst-mor.o
	$(CXX) -o $@ $^ $(LDFLAGS) $(LREADLINE)

fst-text2bin: $(OBJ) fst-text2bin.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%: $(OBJ) %.o
	$(CXX) -o $@ $^ $(LDFLAGS)

fst-infl2: alphabet.o basic.o compact.o robust.o utf8.o fst-infl2.o
	$(CXX) -o $@ $^ $(LDFLAGS)

fst-infl2-daemon: alphabet.o basic.o compact.o robust.o utf8.o fst-infl2-daemon.o
	$(CXX) -o $@ $^ $(LDFLAGS)

fst-infl3: alphabet.o basic.o utf8.o lowmem.o fst-infl3.o
	$(CXX) -o $@ $^ $(LDFLAGS)

test: test.o
	$(CL) -L. -lsfst -o $@ $^ $(LDFLAGS)

fst-compiler.C: fst.h compact.h make-compact.h interface.h fst-compiler.yy
	bison -d -o $@ fst-compiler.yy
	if [ -f fst-compiler.C.h ]; then mv fst-compiler.C.h fst-compiler.h; fi
	if [ -f fst-compiler.H ]; then mv fst-compiler.H fst-compiler.h; fi

default-scanner.C: fst.h interface.h scanner.ll fst-compiler.C default-scanner.ll
	flex -o$@ default-scanner.ll

utf8-scanner.C: fst.h interface.h scanner.ll fst-compiler.C utf8-scanner.ll
	flex -o$@ utf8-scanner.ll

default-scanner.ll: scanner.ll
	sed -e '/^utf8>/d' -e 's/^default> *//' scanner.ll > default-scanner.ll

utf8-scanner.ll: scanner.ll
	sed -e '/^default>/d' -e 's/^utf8> *//' scanner.ll > utf8-scanner.ll

libsfst.tar.gz: libsfst.so compact.h alphabet.h basic.h fst.h 
	tar -zcf $@ $^

libsfst.so: alphabet.o basic.o utf8.o compact.o fst.o operators.o determinise.o generate.o
	$(CL) -shared -o $@ $^ $(LDFLAGS)

remake: clean $(ALLPROGRAMS)

clean:
	-rm -f *.o *~ Makefile.bak y.tab.h man1/*~ 2>&- > /dev/null

realclean: clean
	-rm -f $(ALLPROGRAMS) test 2>&- > /dev/null

install: $(PROGRAMS)
#	$(INSTALL_DIR) $(DESTDIR)$(PREFIX)bin
	for p in $(PROGRAMS); do $(INSTALL) $$p $(DESTDIR)$(PREFIX)bin/$$p; done

maninstall:
	$(INSTALL_DIR) $(DESTDIR)$(PREFIX)man/man1
	for m in man1/*.1; do $(INSTALL_MANPAGES) $$m $(DESTDIR)$(PREFIX)man/$$m; done


archive: 
	tar -zhcf VERSION-`date '+%y%m%d'`.tar.gz *.ll *.yy *.[Ch] Makefile man1/*

Makefile: *.C *.ll *.yy *.h
	-makedepend -Y -- $(CFLAGS) -- *.C 2>/dev/null 

# DO NOT DELETE

TransducerTest.o: Transducer.h
alphabet.o: utf8.h alphabet.h basic.h sgi.h
basic.o: basic.h
compact.o: compact.h alphabet.h basic.h sgi.h
default-scanner.o: interface.h utf8.h fst.h alphabet.h basic.h sgi.h mem.h
default-scanner.o: fst-compiler.h
determinise.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-compact.o: make-compact.h fst.h alphabet.h basic.h sgi.h mem.h compact.h
fst-compare.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-compiler.o: make-compact.h fst.h alphabet.h basic.h sgi.h mem.h compact.h
fst-compiler.o: interface.h utf8.h
fst-generate.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-infl.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-infl2-daemon.o: Socket.h compact.h alphabet.h basic.h sgi.h
fst-infl2.o: compact.h alphabet.h basic.h sgi.h
fst-infl3.o: lowmem.h alphabet.h basic.h sgi.h
fst-lattice.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-lowmem.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-match.o: compact.h alphabet.h basic.h sgi.h
fst-mor.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-parse.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-parse2.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-print.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-text2bin.o: fst.h alphabet.h basic.h sgi.h mem.h
fst-train.o: compact.h alphabet.h basic.h sgi.h
fst.o: fst.h alphabet.h basic.h sgi.h mem.h
generate.o: fst.h alphabet.h basic.h sgi.h mem.h
interface.o: interface.h utf8.h fst.h alphabet.h basic.h sgi.h mem.h
lowmem.o: lowmem.h alphabet.h basic.h sgi.h
make-compact.o: make-compact.h fst.h alphabet.h basic.h sgi.h mem.h compact.h
operators.o: fst.h alphabet.h basic.h sgi.h mem.h
robust.o: compact.h alphabet.h basic.h sgi.h
test.o: compact.h alphabet.h basic.h sgi.h
utf8-scanner.o: interface.h utf8.h fst.h alphabet.h basic.h sgi.h mem.h
utf8-scanner.o: fst-compiler.h
utf8.o: utf8.h
