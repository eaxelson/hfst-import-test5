## Process this file with automake to produce Makefile.in

## For nested distribution as part of hfst
hfst_lexc_LDADD = ../../../hfst2/libhfst.la libhfstlexc.la
AM_CPPFLAGS = -I../../../ -I ../../../hfst2
AM_CXXFLAGS = -O3

lib_LTLIBRARIES=libhfstlexc.la
bin_PROGRAMS=hfst-lexc
include_HEADERS=\
			   lexc.h \
			   string-munging.h \
			   lexcio.h flex-utils.h \
			   xducer.h xymbol.h xymbol-table.h \
			   xymbol-bridges.h 
hfst_lexc_SOURCES=\
				  hfst-lexc.cc \
				  lexc-parser.yy lexc-lexer.ll \
				  lexcio.cc flex-utils.cc
libhfstlexc_la_SOURCES=\
					   lexc.cc string-munging.cc lexcio.cc \
					   xducer.cc xymbol.cc xymbol-table.cc xymbol-bridges.cc
BUILT_SOURCES=lexc-parser.h

hfst_lexc_CXXFLAGS=-Wno-deprecated

libhfstlexc_la_CXXFLAGS=-Wno-deprecated
## Note to self:
## http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
libhfstlexc_la_LDFLAGS=-version-info 0:0:0
## YACC and FLEX are often a PITA
AM_YFLAGS=-d --name-prefix=hlexc
AM_LFLAGS=--prefix=hlexc --outfile=$(LEX_OUTPUT_ROOT).c

