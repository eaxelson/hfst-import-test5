CXXFLAGS=-Wno-deprecated -g -Wall -Wextra -Werror # -DDEBUG_MAIN

FOMA_DIR=implementations/foma

FOMA_OBJS=$(FOMA_DIR)/apply.o $(FOMA_DIR)/coaccessible.o $(FOMA_DIR)/constructions.o $(FOMA_DIR)/define.o $(FOMA_DIR)/determinize.o $(FOMA_DIR)/dynarray.o $(FOMA_DIR)/extract.o $(FOMA_DIR)/flags.o $(FOMA_DIR)/int_stack.o $(FOMA_DIR)/invert.o $(FOMA_DIR)/io.o $(FOMA_DIR)/lex.cmatrix.o $(FOMA_DIR)/lexcread.o $(FOMA_DIR)/mem.o $(FOMA_DIR)/minimize.o $(FOMA_DIR)/regex.tab.o $(FOMA_DIR)/reverse.o $(FOMA_DIR)/rewrite.o $(FOMA_DIR)/sigma.o $(FOMA_DIR)/spelling.o $(FOMA_DIR)/stack.o $(FOMA_DIR)/structures.o $(FOMA_DIR)/topsort.o $(FOMA_DIR)/utf8.o

SFST_DIR=implementations/SFST/src

SFST_OBJS=$(SFST_DIR)/fst.o $(SFST_DIR)/basic.o $(SFST_DIR)/utf8.o \
$(SFST_DIR)/operators.o $(SFST_DIR)/interface.o $(SFST_DIR)/determinise.o \
$(SFST_DIR)/alphabet.o

OFST_OBJS=./implementations/openfst-1.1/src/lib/*.o

HFST_OBJS=implementations/SymbolDefs.o implementations/GlobalSymbolTable.o \
implementations/HfstTokenizer.o implementations/ConvertTransducerFormat.o \
HfstApply.o HfstInputStream.o HfstTransducer.o HfstOutputStream.o

HFST_OBJS_FOR_HFST_TRANSDUCER=implementations/SymbolDefs.o implementations/GlobalSymbolTable.o \
implementations/HfstTokenizer.o implementations/ConvertTransducerFormat.o \
HfstApply.o HfstInputStream.o

IMPLEMENTATIONS=implementations/SfstTransducer.o \
implementations/TropicalWeightTransducer.o \
implementations/LogWeightTransducer.o \
implementations/LogFstTrieFunctions.o \
implementations/FomaTransducer.o

OFST_LIB_PATHS=-Limplementations/openfst-1.1/src/lib \
-Limplementations/openfst-1.1/src/bin

all: objs
	ar rc libhfst3.a $(SFST_OBJS) $(OFST_OBJS) $(FOMA_OBJS) \
	$(IMPLEMENTATIONS) $(HFST_OBJS)


objs: HfstInputStream.o HfstOutputStream.o HfstTransducer.o HfstApply.o hfst_obj

clean:
	rm -Rf *.o libhfst3.a
	$(MAKE) -C ./implementations clean

HfstTransducer: HfstTransducer.o HfstInputStream.o HfstApply.o $(HFST_OBJS) $(IMPLEMENTATIONS)
	g++ -Wno-deprecated -g $(OFST_LIB_PATHS) -lfstmain -lfst\
	 -ldl -lm -o $@ HfstTransducer.cc $(SFST_OBJS) $(HFST_OBJS_FOR_HFST_TRANSDUCER) $(IMPLEMENTATIONS)

HfstInputStream: HfstInputStream.cc
	g++ -g $(OFST_LIB_PATHS) -lfstmain -lfst -ldl -lm \
	-o $@ $^ $(SFST_OBJS) $(HFST_OBJS) $(IMPLEMENTATIONS)

hfst_obj:
	$(MAKE) -C ./implementations all

test.o: test.cc
test: test.o libhfst3.a

test.o: 
	g++ -I. -c test.cc -o test.o

test:
	g++ -i test.o -lhfst3 -lz -L. -o test

