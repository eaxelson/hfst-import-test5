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


AM_CPPFLAGS = -I${top_srcdir}/libhfst/src -I${top_srcdir}/lib -I${top_builddir}/lib

noinst_LTLIBRARIES=libhfstsillyparsers.la

XRE_SRCS=xre_lex.ll xre_parse.yy xre_utils.cc XreCompiler.cc
XRE_HDRS=XreCompiler.h xre_utils.h
XRE_BUILT=xre_parse.h
XRE_TSTS=XreCompiler
LEXC_SRCS=lexc-lexer.ll lexc-parser.yy lexc-utils.cc LexcCompiler.cc
LEXC_HDRS=LexcCompiler.h lexc-utils.h
LEXC_BUILT=lexc-parser.h
LEXC_TSTS=LexcCompiler
XFST_SRCS=xfst-lexer.ll xfst-parser.yy xfst-utils.cc XfstCompiler.cc
XFST_HDRS=XfstCompiler.h xfst-utils.h
XFST_BUILT=xfst-parser.h
XFST_TSTS=XfstCompiler
BUILT_SOURCES=$(XRE_BUILT) $(LEXC_BUILT) $(XFST_BUILT)

AM_YFLAGS=-d
AM_LFLAGS=--outfile=$(LEX_OUTPUT_ROOT).c

libhfstsillyparsers_la_SOURCES=$(SFST_SRCS) $(XRE_SRCS) $(LEXC_SRCS) $(XFST_SRCS)


check_PROGRAMS=$(XRE_TSTS) $(LEXC_TSTS) $(XFST_TSTS)
LexcCompiler_SOURCES=LexcCompiler.cc
LexcCompiler_CXXFLAGS=-DDEBUG_MAIN
LexcCompiler_LDADD=../libhfst.la
XreCompiler_SOURCES=XreCompiler.cc
XreCompiler_CXXFLAGS=-DDEBUG_MAIN
XreCompiler_LDADD=../libhfst.la
XfstCompiler_SOURCES=XfstCompiler.cc
XfstCompiler_LDADD=../libhfst.la
XfstCompiler_CXXFLAGS=-DDEBUG_MAIN
TESTS=$(XRE_TSTS) $(LEXC_TSTS) $(XFST_TSTS)
