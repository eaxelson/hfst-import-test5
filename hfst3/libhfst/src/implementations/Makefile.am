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
	ConvertTransducerFormat.cc HfstInternalTransducer.cc
AM_CXXFLAGS=-Wno-deprecated
# the bridges
if WANT_SFST
MAYBE_SFST=SfstTransducer.cc
endif
if WANT_OPENFST
MAYBE_OPENFST=TropicalWeightTransducer.cc LogWeightTransducer.cc \
			  LogFstTrieFunctions.cc TropicalWeightComposeIntersect.cc
endif
if WANT_FOMA
MAYBE_FOMA=FomaTransducer.cc
endif
if WANT_MFSTL
MAYBE_MFSTL=MfstlTransducer.cc
endif
if WANT_HFSTOL
MAYBE_HFSTOL=HfstOlTransducer.cc
endif

BRIDGE_SRCS=$(MAYBE_SFST) $(MAYBE_OPENFST) $(MAYBE_FOMA) $(MAYBE_MFSTL) $(MAYBE_HFSTOL)

if WANT_HFSTOL
HFST_OL_SRCS=\
optimized-lookup/transducer.cc optimized-lookup/convert.cc
endif

libhfstimplementations_la_SOURCES=$(IMPLEMENTATION_SRCS) $(BRIDGE_SRCS) \
								  $(SFST_SRCS) $(HFST_OL_SRCS)

AM_CPPFLAGS = -I..

# headers of implementation bridge to reveal to public
hfstincludedir = $(includedir)/hfst
implincludedir = $(hfstincludedir)/implementations

implinclude_HEADERS = \
		ConvertTransducerFormat.h \
		FomaTransducer.h \
		LogFstTrieFunctions.h LogWeightTransducer.h \
		SfstTransducer.h TropicalWeightTransducer.h HfstOlTransducer.h HfstInternalTransducer.h \
		MfstlTransducer.h


if WANT_HFSTOL
hfstolincludedir = $(implincludedir)/optimized-lookup
hfstolinclude_HEADERS=optimized-lookup/transducer.h
endif

