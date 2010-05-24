#SUBDIRS=foma SFST/src openfst-1.1
AUTOMAKE_OPTIONS=subdir-objects
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

# underlying backend objects
SFST_SRCS=\
	SFST/src/alphabet.C         \
	SFST/src/basic.C            \
	SFST/src/compact.C          SFST/src/fst.C     SFST/src/generate.C\
	SFST/src/interface.C\
	SFST/src/determinise.C      SFST/src/lowmem.C\
	SFST/src/make-compact.C\
	SFST/src/operators.C\
	SFST/src/robust.C           SFST/src/utf8.C    SFST/src/expand.C
OFST_LIBS=openfst-1.1/src/lib/libfst.la
OFST_SRCS=\
		  openfst-1.1/src/lib/compat.cc \
		  openfst-1.1/src/lib/fst.cc \
		  openfst-1.1/src/lib/properties.cc \
		  openfst-1.1/src/lib/symbol-table.cc \
		  openfst-1.1/src/lib/flags.cc \
		  openfst-1.1/src/lib/util.cc 
FOMA_SRCS=\
foma/apply.c          foma/flags.c        foma/spelling.c\
foma/coaccessible.c   foma/int_stack.c    foma/mem.c        foma/structures.c\
foma/constructions.c  foma/invert.c       foma/minimize.c   foma/topsort.c\
foma/define.c         foma/io.c           foma/utf8.c\
foma/determinize.c    foma/lex.cmatrix.c  foma/reverse.c\
foma/dynarray.c       foma/lexcread.c     foma/rewrite.c\
foma/extract.c        foma/lex.lexc.c     foma/sigma.c


libhfstimplementations_la_SOURCES=$(IMPLEMENTATION_SRCS) $(BRIDGE_SRCS) \
								  $(SFST_SRCS) $(FOMA_SRCS) $(OFST_SRCS)

# should work like so:
# libhfstimplementations_la_LIBADD=$(OFST_LIBS)

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
