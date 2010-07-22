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
# HFST bridge specific stuff
IMPLEMENTATION_SRCS= \
	HfstTokenizer.cc ConvertTransducerFormat.cc # GlobalSymbolTable.cc SymbolDefs.cc

# the bridges
if WANT_SFST
MAYBE_SFST=SfstTransducer.cc
endif
if WANT_OPENFST
MAYBE_OPENFST=TropicalWeightTransducer.cc LogWeightTransducer.cc \
			  LogFstTrieFunctions.cc
endif
if WANT_FOMA
MAYBE_FOMA=FomaTransducer.cc
endif
if WANT_HFSTOL
MAYBE_HFSTOL=HfstOlTransducer.cc
endif

BRIDGE_SRCS=$(MAYBE_SFST) $(MAYBE_OPENFST) $(MAYBE_FOMA) $(MAYBE_HFSTOL)

# underlying backend objects
if WANT_SFST
SFST_SRCS=\
	SFST/src/alphabet.C         \
	SFST/src/basic.C            \
	SFST/src/compact.C          SFST/src/fst.C     SFST/src/generate.C\
	SFST/src/interface.C\
	SFST/src/determinise.C      SFST/src/lowmem.C\
	SFST/src/make-compact.C\
	SFST/src/operators.C\
	SFST/src/robust.C           SFST/src/utf8.C    SFST/src/expand.C
endif
if WANT_FOMA
FOMA_SRCS=\
		  foma/apply.c          foma/flags.c        foma/spelling.c\
		  foma/coaccessible.c   foma/int_stack.c    foma/mem.c\
		  foma/structures.c foma/constructions.c  foma/invert.c\
		  foma/minimize.c foma/topsort.c foma/define.c         foma/io.c\
		  foma/utf8.c foma/determinize.c    foma/lex.cmatrix.c\
		  foma/reverse.c foma/dynarray.c foma/lexcread.c     foma/rewrite.c\
		  foma/extract.c        foma/lex.lexc.c foma/sigma.c
endif
if WANT_HFSTOL
HFST_OL_SRCS=\
optimized-lookup/transducer.cc optimized-lookup/convert.cc
endif

libhfstimplementations_la_SOURCES=$(IMPLEMENTATION_SRCS) $(BRIDGE_SRCS) \
								  $(SFST_SRCS) $(FOMA_SRCS) $(HFST_OL_SRCS)

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
if WANT_SFST
sfstincludedir = $(implincludedir)/SFST/src
SFST_DIR = SFST/src
sfstinclude_HEADERS = \
		$(SFST_DIR)/alphabet.h $(SFST_DIR)/basic.h $(SFST_DIR)/compact.h \
		$(SFST_DIR)/make-compact.h $(SFST_DIR)/fst.h $(SFST_DIR)/mem.h \
		$(SFST_DIR)/lowmem.h \
		$(SFST_DIR)/utf8.h 
endif

if WANT_HFSTOL
hfstolincludedir = $(implincludedir)/optimized-lookup
hfstolinclude_HEADERS=optimized-lookup/transducer.h
endif

