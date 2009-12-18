CXXFLAGS=-Wno-deprecated -g -O3

SFST_DIR=implementations/SFST/src

SFST_OBJS=$(SFST_DIR)/fst.o $(SFST_DIR)/basic.o $(SFST_DIR)/utf8.o \
$(SFST_DIR)/operators.o $(SFST_DIR)/interface.o $(SFST_DIR)/determinise.o \
$(SFST_DIR)/alphabet.o

OFST_OBJS=./implementations/openfst-1.1/src/lib/*.o

HFST_OBJS=implementations/SymbolDefs.o implementations/GlobalSymbolTable.o \
implementations/HfstTokenizer.o implementations/ConvertTransducerFormat.o \
HfstApply.o HfstInputStream.o HfstTransducer.o

IMPLEMENTATIONS=implementations/SfstTransducer.o \
implementations/TropicalWeightTransducer.o

OFST_LIB_PATHS=-Limplementations/openfst-1.1/src/lib \
-Limplementations/openfst-1.1/src/bin

all: objs
	ar rc libhfst3.a $(SFST_OBJS) $(OFST_OBJS) \
	$(IMPLEMENTATIONS) $(HFST_OBJS)


objs: HfstInputStream.o HfstTransducer.o HfstApply.o hfst_obj

clean:
	rm -Rf *.o libhfst3.a
	$(MAKE) -C ./implementations clean

HfstTransducer: HfstTransducer.cc HfstInputStream.o HfstApply.o
	g++ -Wno-deprecated -g -DDEBUG_MAIN $(OFST_LIB_PATHS) -lfstmain -lfst\
	 -ldl -lm -o $@ $^ $(SFST_OBJS) $(HFST_OBJS) $(IMPLEMENTATIONS)

HfstInputStream: HfstInputStream.cc
	g++ -g -DDEBUG_MAIN $(OFST_LIB_PATHS) -lfstmain -lfst -ldl -lm \
	-o $@ $^ $(SFST_OBJS) $(HFST_OBJS) $(IMPLEMENTATIONS)

hfst_obj:
	$(MAKE) -C ./implementations all