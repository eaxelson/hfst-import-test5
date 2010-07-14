## Process this file with automake to produce Makefile.in

## Copyright (C) 2010 University of Helsinki

## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

AUTOMAKE_OPTIONS=subdir-objects
noinst_LTLIBRARIES=libhfstimplementations.la
AM_CPPFLAGS=-Iopenfst-1.1/src/include/
# HFST bridge specific stuff
IMPLEMENTATION_SRCS= \
	HfstTokenizer.cc ConvertTransducerFormat.cc # GlobalSymbolTable.cc SymbolDefs.cc

# the bridges
BRIDGE_SRCS=SfstTransducer.cc \
	TropicalWeightTransducer.cc \
	LogWeightTransducer.cc \
	LogFstTrieFunctions.cc \
	FomaTransducer.cc \
	HfstOlTransducer.cc

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

HFST_OL_SRCS=\
optimized-lookup/transducer.cc


libhfstimplementations_la_SOURCES=$(IMPLEMENTATION_SRCS) $(BRIDGE_SRCS) \
								  $(SFST_SRCS) $(FOMA_SRCS) $(HFST_OL_SRCS) # $(OFST_SRCS)

# should work like so:
# libhfstimplementations_la_LIBADD=$(OFST_LIBS)

# headers of implementation bridge to reveal to public
hfstincludedir = $(includedir)/hfst
implincludedir = $(hfstincludedir)/implementations

implinclude_HEADERS = \
		ConvertTransducerFormat.h \
		ExtractStrings.h FomaTransducer.h HfstExceptions.h \
		HfstTokenizer.h LogFstTrieFunctions.h LogWeightTransducer.h \
		SfstTransducer.h TropicalWeightTransducer.h HfstOlTransducer.h # GlobalSymbolTable.h SymbolDefs.h

# headers of backends to reveal to public...
# TODO: in ideal case you'd delete these and link to public libs of backends
# using public headers of backends
sfstincludedir = $(implincludedir)/SFST/src
ofstincludedir = $(implincludedir)/openfst-1.1/src/include/fst
fomaincludedir = $(implincludedir)/foma
hfstolincludedir = $(implincludedir)/optimized-lookup
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

HFST_OL_DIR = optimized-lookup
HFST_OL_HDRS = \
		optimized-lookup/transducer.h

hfstinclude_HEADERS = $(HFST_HDRS)
sfstinclude_HEADERS = $(SFST_HDRS)
fomainclude_HEADERS = $(FOMA_HDRS)
hfstolinclude_HEADERS = $(HFST_OL_HDRS)
