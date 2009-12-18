SOURCES=SfstTransducer.h SymbolDefs.h TropicalWeightTransducer.h GlobalSymbolTable.h HfstTokenier.h ConvertTransducerFormat.h \
HfstException.h SfstTransducer.cc SymbolDefs.cc TropicalWeightTransducer.cc HfstTokenizer.cc ConvertTransducerFormat.cc
CXXFLAGS=-O2 -Wall -Wextra
all: SfstTransducer.o TropicalWeightTransducer.o SymbolDefs.o GlobalSymbolTable.o HfstTokenizer.o ConvertTransducerFormat.o ofst sfst

clean:
	rm -f SfstTransducer TropicalWeightTransducer *.o
	$(MAKE) -C SFST/src/ clean
	$(MAKE) -C openfst-1.1 clean

SfstTransducer: SfstTransducer.cc SymbolDefs.o GlobalSymbolTable.o
	g++ -g -DDEBUG -DDEBUG_MAIN -O3 -o $@ $^ SFST/src/fst.o SFST/src/basic.o \
	SFST/src/interface.o SFST/src/utf8.o SFST/src/determinise.o \
	SFST/src/operators.o SFST/src/alphabet.o

TropicalWeightTransducer: TropicalWeightTransducer.cc SymbolDefs.o GlobalSymbolTable.o
	g++ -g -DDEBUG -DDEBUG_MAIN -Lopenfst-1.1/src/lib -Lopenfst-1.1/src/bin -lfstmain \
	-lfst -ldl -O3 -o $@ $^

SfstTransducer.o: SfstTransducer.cc
	g++ -g $(CXXFLAGS) -Wno-deprecated -c -o $@ $^

ConvertTransducerFormat.o: ConvertTransducerFormat.cc
	g++ -g $(CXXFLAGS) -Wno-deprecated -c -o $@ $^

ofst:
	$(MAKE) -C ./openfst-1.1 all

sfst:
	$(MAKE) -C ./SFST/src all