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
IMPLEMENTATION_SRCS=HfstTransitionGraph.cc ConvertTransducerFormat.cc
AM_CXXFLAGS=-Wno-deprecated
AM_CPPFLAGS = -I${top_srcdir}/libhfst/src
# the bridges
if WANT_SFST
MAYBE_SFST=SfstTransducer.cc
endif
if WANT_OPENFST
MAYBE_OPENFST=TropicalWeightTransducer.cc LogWeightTransducer.cc
endif
if WANT_FOMA
MAYBE_FOMA=FomaTransducer.cc
endif
# Add here your transducer library
#if WANT_MY_TRANSDUCER_LIBRARY
#MAYBE_MY_TRANSDUCER_LIBRARY=MyTransducerLibraryTransducer.cc
#endif
if WANT_HFSTOL
MAYBE_HFSTOL=HfstOlTransducer.cc
endif

BRIDGE_SRCS=$(MAYBE_SFST) $(MAYBE_OPENFST) $(MAYBE_FOMA) $(MAYBE_HFSTOL) # $(MAYBE_MY_TRANSDUCER_LIBRARY)

if WANT_HFSTOL
HFST_OL_SRCS=\
optimized-lookup/transducer.cc optimized-lookup/convert.cc
endif

libhfstimplementations_la_SOURCES=$(IMPLEMENTATION_SRCS) $(BRIDGE_SRCS) \
								  $(SFST_SRCS) $(HFST_OL_SRCS)


# headers of implementation bridge to reveal to public
hfstincludedir = $(includedir)/hfst
implincludedir = $(hfstincludedir)/implementations

implinclude_HEADERS = \
		ConvertTransducerFormat.h \
		FomaTransducer.h \
		LogWeightTransducer.h \
		SfstTransducer.h TropicalWeightTransducer.h \
		HfstOlTransducer.h HfstTransitionGraph.h
### Add here your library ###
#		MyTransducerLibraryTransducer.h


if WANT_HFSTOL
hfstolincludedir = $(implincludedir)/optimized-lookup
hfstolinclude_HEADERS = \
		optimized-lookup/transducer.h \
		optimized-lookup/convert.h
endif

if WANT_FOMA
FOMA_TSTS=FomaTransducer
endif
if WANT_HFSTOL
HFSTOL_TSTS=HfstOlTransducer
endif
if WANT_OPENFST
OFST_TSTS=LogWeightTransducer TropicalWeightTransducer
endif
if WANT_SFST
SFST_TSTS=SfstTransducer
endif
# Add here your library test
#if WANT_MY_TRANSDUCER_LIBRARY
#MY_TRANSDUCER_LIBRARY_TSTS=MyTransducerLibraryTransducer
#endif
check_PROGRAMS=ConvertTransducerFormat \
			   $(FOMA_TSTS) $(HFSTOL_TSTS) $(OFST_TSTS) $(SFST_TSTS) # $(MY_TRANSDUCER_LIBRARY_TSTS)
ConvertTransducerFormat_SOURCES=ConvertTransducerFormat.cc
ConvertTransducerFormat_CXXFLAGS=-DDEBUG_MAIN
ConvertTransducerFormat_LDADD=../libhfst.la
FomaTransducer_SOURCES=FomaTransducer.cc
FomaTransducer_CXXFLAGS=-DDEBUG_MAIN
FomaTransducer_LDADD=../libhfst.la
HfstOlTransducer_SOURCES=HfstOlTransducer.cc
HfstOlTransducer_CXXFLAGS=-DDEBUG_MAIN
HfstOlTransducer_LDADD=../libhfst.la
LogWeightTransducer_SOURCES=LogWeightTransducer.cc
LogWeightTransducer_CXXFLAGS=-DDEBUG_MAIN
LogWeightTransducer_LDADD=../libhfst.la
### Add here your library tests ###
# MyTransducerLibraryTransducer_SOURCES=MyTransducerLibraryTransducer.cc
# MyTransducerLibraryTransducer_CXXFLAGS=-DDEBUG_MAIN
# MyTransducerLibraryTransducer_LDADD=../libhfst.la
SfstTransducer_SOURCES=SfstTransducer.cc
SfstTransducer_CXXFLAGS=-DDEBUG_MAIN
SfstTransducer_LDADD=../libhfst.la
TropicalWeightTransducer_SOURCES=TropicalWeightTransducer.cc
TropicalWeightTransducer_CXXFLAGS=-DDEBUG_MAIN
TropicalWeightTransducer_LDADD=../libhfst.la
TESTS=ConvertTransducerFormat \
	  $(FOMA_TSTS) $(HFSTOL_TSTS) $(OFST_TSTS) $(SFST_TSTS) # $(MY_TRANSDUCER_LIBRARY_TSTS)

