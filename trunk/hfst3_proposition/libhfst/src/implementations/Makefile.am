SUBDIRS=foma SFST/src openfst-1.1
noinst_LTLIBRARIES=libhfstimplementations.la
# HFST bridge specific stuff
IMPLEMENTATION_SRCS=SymbolDefs.cc GlobalSymbolTable.cc \
	HfstTokenizer.cc ConvertTransducerFormat.cc

# the bridges
BRIDGE_SRCS=SfstTransducer.cc \
	TropicalWeightTransducer.cc \
	LogWeightTransducer.cc \
	LogFstTrieFunctions.cc \
	FomaTransducer.cc

libhfstimplementations_la_SOURCES=$(IMPLEMENTATION_SRCS) $(BRIDGE_SRCS)

# headers of implementation bridge to reveal to public
hfstincludedir = $(includedir)/hfst
implincludedir = $(hfstincludedir)/implementations

implinclude_HEADERS = \
		ConvertTransducerFormat.h \
		ExtractStrings.h FomaTransducer.h GlobalSymbolTable.h HfstExceptions.h \
		HfstTokenizer.h LogFstTrieFunctions.h LogWeightTransducer.h \
		SfstTransducer.h SymbolDefs.h TropicalWeightTransducer.h

# headers of backends to reveal to public...
# TODO: in ideal case you'd delete these and link to public libs of backends
# using public headers of backends
sfstincludedir = $(implincludedir)/SFST/src
ofstincludedir = $(implincludedir)/openfst-1.1/src/include/fst
fomaincludedir = $(implincludedir)/foma
SFST_DIR = SFST/src
SFST_HDRS = \
		$(SFST_DIR)/alphabet.h $(SFST_DIR)/basic.h $(SFST_DIR)/compact.h \
		$(SFST_DIR)/make-compact.h $(SFST_DIR)/fst.h $(SFST_DIR)/mem.h \
		$(SFST_DIR)/lowmem.h \
		$(SFST_DIR)/utf8.h
# $(SFST_DIR)/tokenize_functions.h 
# $(SFST_DIR)/binSearch.h $(SFST_DIR)/transducer-conversion.h
# $(SFST_DIR)/lookup.h $(SFST_DIR)/fst-lex.h $(SFST_DIR)/fst-regression.h
# $(SFST_DIR)/runtime_conversion.h 
# $(SFST_DIR)/read_runtime_transducer.h
# $(SFST_DIR)/runtime_type_defs.h


OFST_DIR = openfst-1.1/src/include/fst/
OFST_HDRS = \
	$(OFST_DIR)/*.h

FOMA_HDRS = \
		foma/foma.h
		foma/fomalibconf.h
		foma/fomalib.h
		foma/lexc.h
		foma/regex.tab.h

hfstinclude_HEADERS = $(HFST_HDRS)
sfstinclude_HEADERS = $(SFST_HDRS)
fomainclude_HEADERS = $(FOMA_HDRS)
