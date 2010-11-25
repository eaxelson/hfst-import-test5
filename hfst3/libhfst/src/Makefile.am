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

SUBDIRS=implementations extensions
AUTOMAKE_OPTIONS=subdir-objects
lib_LTLIBRARIES = libhfst.la

AM_CXXFLAGS=-Wno-deprecated -Wall -Wextra

# HFST bridge specific stuff
HFST_SRCS=HfstApply.cc HfstInputStream.cc HfstTransducer.cc HfstOutputStream.cc\
		  HfstRules.cc HfstBasic.cc HfstUtf8.cc \
		  HfstGrammar.cc SymbolDefs.cc HfstTokenizer.cc \
		  FlagDiacritics.cc HfstAlphabet.cc

# libtool takes over
libhfst_la_SOURCES =  $(HFST_SRCS)
libhfst_la_LIBADD = implementations/libhfstimplementations.la\
					extensions/libhfstextensions.la

AM_CPPFLAGS = -I./implementations

# we only have one library target but would like a particular includedir
# structure for various headers, so we do this
hfstincludedir = $(includedir)/hfst

HFST_HDRS = \
	HfstTransducer.h \
	apply_schemas.h

hfstinclude_HEADERS = $(HFST_HDRS)

libhfst_la_LDFLAGS = -no-undefined -version-info 1:0:0
