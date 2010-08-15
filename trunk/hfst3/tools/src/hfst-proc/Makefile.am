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

LDADD = $(top_builddir)/libhfst/src/libhfst.la
AM_CPPFLAGS = -I${top_srcdir}/libhfst/src -I${top_srcdir}/lib -I${top_builddir}/lib
AM_CXXFLAGS = $(WERR_CFLAGS)

if WANT_PROC
MAYBE_PROC=hfst-proc
endif

noinst_HEADERS = \
	hfst-proc.h formatter.h lookup-path.h lookup-state.h tokenizer.h \
	transducer.h buffer.h applicators.h alphabet.h

bin_PROGRAMS=$(MAYBE_PROC)
hfst_proc_SOURCES = hfst-proc.cc formatter.cc lookup-path.cc lookup-state.cc tokenizer.cc transducer.cc applicators.cc alphabet.cc

