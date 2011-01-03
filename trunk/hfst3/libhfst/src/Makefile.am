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

EXTRA_DIST=coding-style.sh

AM_CXXFLAGS=-Wno-deprecated -Wall -Wextra

# HFST bridge specific stuff
HFST_SRCS=HfstApply.cc HfstInputStream.cc HfstTransducer.cc HfstOutputStream.cc\
		  HfstRules.cc HfstBasic.cc HfstUtf8.cc \
		  HfstGrammar.cc HfstSymbolDefs.cc HfstTokenizer.cc \
		  HfstFlagDiacritics.cc HfstAlphabet.cc HfstExceptions.cc

# libtool takes over
libhfst_la_SOURCES =  $(HFST_SRCS)
libhfst_la_LIBADD = implementations/libhfstimplementations.la

AM_CPPFLAGS = -I./implementations

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
	implementations/MfstlTransducer.h \
	HfstAlphabet.h \
	implementations/HfstOlTransducer.h \
	HfstTokenizer.h \
	implementations/ConvertTransducerFormat.h \
	implementations/HfstNet.h \
	HfstExceptions.h \
	HfstExtractStrings.h \
	HfstFlagDiacritics.h \
	HfstInputStream.h \
	HfstOutputStream.h

hfstinclude_HEADERS = $(HFST_HDRS)

libhfst_la_LDFLAGS = -no-undefined -version-info 1:0:0



# unit tests: add one per class/file/anything

check_PROGRAMS = HfstTransducer HfstFlagDiacritics HfstGrammar HfstInputStream \
				 HfstOutputStream HfstTokenizer
HfstTransducer_SOURCES=HfstTransducer.cc
HfstTransducer_CXXFLAGS=-DDEBUG_MAIN
HfstTransducer_LDADD=libhfst.la
HfstFlagDiacritics_SOURCES=HfstFlagDiacritics.cc
HfstFlagDiacritics_CXXFLAGS=-DDEBUG_MAIN
HfstFlagDiacritics_LDADD=libhfst.la
HfstGrammar_SOURCES=HfstGrammar.cc
HfstGrammar_CXXFLAGS=-DDEBUG_MAIN
HfstGrammar_LDADD=libhfst.la
HfstInputStream_SOURCES=HfstInputStream.cc
HfstInputStream_CXXFLAGS=-DDEBUG_MAIN
HfstInputStream_LDADD=libhfst.la
HfstOutputStream_SOURCES=HfstOutputStream.cc
HfstOutputStream_CXXFLAGS=-DDEBUG_MAIN
HfstOutputStream_LDADD=libhfst.la
HfstTokenizer_SOURCES=HfstTokenizer.cc
HfstTokenizer_CXXFLAGS=-DDEBUG_MAIN
HfstTokenizer_LDADD=libhfst.la
TESTS=HfstTransducer HfstFlagDiacritics HfstGrammar HfstInputStream \
	  HfstOutputStream HfstTokenizer \
	  coding-style.sh
