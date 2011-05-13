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

SUBDIRS=implementations
AUTOMAKE_OPTIONS=subdir-objects
lib_LTLIBRARIES = libhfst.la

AM_CXXFLAGS=-Wno-deprecated -Wall -Wextra -g
AM_CPPFLAGS = -I${top_srcdir}/libhfst/src

# HFST bridge specific stuff
HFST_SRCS=HfstApply.cc HfstInputStream.cc HfstTransducer.cc HfstOutputStream.cc\
		  HfstRules.cc \
		  HfstSymbolDefs.cc HfstTokenizer.cc \
		  HfstFlagDiacritics.cc HfstExceptionDefs.cc \
		  HarmonizeUnknownAndIdentitySymbols.cc

# libtool takes over
libhfst_la_SOURCES =  $(HFST_SRCS)
libhfst_la_LIBADD = implementations/libhfstimplementations.la

# we only have one library target but would like a particular includedir
# structure for various headers, so we do this
hfstincludedir = $(includedir)/hfst

HFST_HDRS = \
	HfstTransducer.h \
	hfst_apply_schemas.h \
	HfstDataTypes.h \
	HfstSymbolDefs.h \
	implementations/SfstTransducer.h \
	implementations/TropicalWeightTransducer.h \
	implementations/LogWeightTransducer.h \
	implementations/FomaTransducer.h \
	implementations/HfstOlTransducer.h \
	HfstTokenizer.h \
	implementations/ConvertTransducerFormat.h \
	implementations/HfstTransitionGraph.h \
	implementations/HfstTropicalTransducerTransitionData.h \
	implementations/HfstFastTransitionData.h \
	implementations/compose_intersect/ComposeIntersectRulePair.h \
	implementations/compose_intersect/ComposeIntersectLexicon.h \
	implementations/compose_intersect/ComposeIntersectRule.h \
	implementations/compose_intersect/ComposeIntersectFst.h \
	HarmonizeUnknownAndIdentitySymbols.h \
	HfstExceptionDefs.h \
	HfstExtractStrings.h \
	HfstFlagDiacritics.h \
	HfstInputStream.h \
	HfstOutputStream.h
### Add your library here ###
#	implementations/MyTransducerLibraryTransducer.h

hfstinclude_HEADERS = $(HFST_HDRS)

libhfst_la_LDFLAGS = -no-undefined -version-info 3:0:2

LIBHFST_TSTS=HfstApply HfstInputStream HfstTransducer \
		HfstOutputStream HfstRules HfstSymbolDefs \
		HfstTokenizer HfstFlagDiacritics \
		HarmonizeUnknownAndIdentitySymbols

check_PROGRAMS=$(LIBHFST_TSTS)

HfstApply_SOURCES=HfstApply.cc
HfstApply_CXXFLAGS=-DMAIN_TEST
HfstApply_LDADD=libhfst.la
HfstInputStream_SOURCES=HfstInputStream.cc
HfstInputStream_CXXFLAGS=-DMAIN_TEST
HfstInputStream_LDADD=libhfst.la
HfstTransducer_SOURCES=HfstTransducer.cc
HfstTransducer_CXXFLAGS=-DMAIN_TEST
HfstTransducer_LDADD=libhfst.la
HfstOutputStream_SOURCES=HfstOutputStream.cc
HfstOutputStream_CXXFLAGS=-DMAIN_TEST
HfstOutputStream_LDADD=libhfst.la
HfstRules_SOURCES=HfstRules.cc
HfstRules_CXXFLAGS=-DMAIN_TEST
HfstRules_LDADD=libhfst.la
HfstSymbolDefs_SOURCES=HfstSymbolDefs.cc
HfstSymbolDefs_CXXFLAGS=-DMAIN_TEST
HfstSymbolDefs_LDADD=libhfst.la
HfstTokenizer_SOURCES=HfstTokenizer.cc
HfstTokenizer_CXXFLAGS=-DMAIN_TEST
HfstTokenizer_LDADD=libhfst.la
HfstFlagDiacritics_SOURCES=HfstFlagDiacritics.cc
HfstFlagDiacritics_CXXFLAGS=-DMAIN_TEST
HfstFlagDiacritics_LDADD=libhfst.la
HarmonizeUnknownAndIdentitySymbols_SOURCES=HarmonizeUnknownAndIdentitySymbols.cc
HarmonizeUnknownAndIdentitySymbols_CXXFLAGS=-DMAIN_TEST
HarmonizeUnknownAndIdentitySymbols_LDADD=libhfst.la

TESTS=$(LIBHFST_TSTS)