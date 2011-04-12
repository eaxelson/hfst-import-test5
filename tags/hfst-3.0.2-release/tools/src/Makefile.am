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

SUBDIRS=hfst-proc hfst-twolc
AUTOMAKE_OPTIONS=std-options subdir-objects
LDADD = $(top_builddir)/libhfst/src/libhfst.la
AM_CPPFLAGS = -I${top_srcdir}/libhfst/src -I${top_srcdir}/tools/src/parsers
AM_CXXFLAGS = -Wno-deprecated

MAYBE_ALIASES=
# sort alphabetically:
if WANT_COMPARE
MAYBE_COMPARE=hfst-compare
endif
if WANT_COMPOSE
MAYBE_COMPOSE=hfst-compose
endif
if WANT_COMPOSE_INTERSECT
MAYBE_COMPOSE_INTERSECT=hfst-compose-intersect
endif
if WANT_CONCATENATE
MAYBE_CONCATENATE=hfst-concatenate
endif
if WANT_CONJUNCT
MAYBE_CONJUNCT=hfst-conjunct
MAYBE_ALIASES+=hfst-intersect
endif
if WANT_DETERMINIZE
MAYBE_DETERMINIZE=hfst-determinize
MAYBE_ALIASES+=hfst-determinise
endif
if WANT_DISJUNCT
MAYBE_DISJUNCT=hfst-disjunct
MAYBE_ALIASES+=hfst-union
endif
if WANT_FORMAT
MAYBE_FORMAT=hfst-format
endif
if WANT_FST2FST
MAYBE_FST2FST=hfst-fst2fst
endif
if WANT_FST2STRINGS
MAYBE_FST2STRINGS=hfst-fst2strings
MAYBE_ALIASES+=hfst-expand
endif
if WANT_FST2TXT
MAYBE_FST2TXT=hfst-fst2txt
endif
if WANT_HEAD
MAYBE_HEAD=hfst-head
endif
if WANT_INVERT
MAYBE_INVERT=hfst-invert
endif
if WANT_LEXC_COMPILER
MAYBE_LEXC_COMPILER=hfst-lexc2fst
endif
if WANT_LOOKUP
MAYBE_LOOKUP=hfst-lookup
MAYBE_ALIASES+=hfst-optimized-lookup hfst-optimised-lookup
endif
if WANT_PAIR_TEST
MAYBE_PAIR_TEST=hfst-pair-test
endif
if WANT_MINIMIZE
MAYBE_MINIMIZE=hfst-minimize
MAYBE_ALIASES+=hfst-minimise
endif
if WANT_NAME
MAYBE_NAME=hfst-name
endif
if WANT_PREPROCESS_FOR_OPTIMIZED_LOOKUP
MAYBE_PREPROCESS_FOR_OPTIMIZED_LOOKUP=hfst-preprocess-for-optimized-lookup-format
MAYBE_ALIASES+=hfst-preprocess-for-optimised-lookup-format
endif
if WANT_PROJECT
MAYBE_PROJECT=hfst-project
endif
if WANT_PUSH_WEIGHTS
MAYBE_PUSH_WEIGHTS=hfst-push-weights
endif
if WANT_REGEXP2FST
MAYBE_REGEXP2FST=hfst-regexp2fst
endif
if WANT_REMOVE_EPSILONS
MAYBE_REMOVE_EPSILONS=hfst-remove-epsilons
endif
if WANT_REPEAT
MAYBE_REPEAT=hfst-repeat
endif
if WANT_REVERSE
MAYBE_REVERSE=hfst-reverse
endif
if WANT_SPLIT
MAYBE_SPLIT=hfst-split
endif
if WANT_STRINGS2FST
MAYBE_STRINGS2FST=hfst-strings2fst
endif
if WANT_STRIP_HEADER
MAYBE_STRIP_HEADER=hfst-strip-header
endif
if WANT_SUBTRACT
MAYBE_SUBTRACT=hfst-subtract
MAYBE_ALIASES+=hfst-minus
endif
if WANT_SUMMARIZE
MAYBE_SUMMARIZE=hfst-summarize
MAYBE_ALIASES+=hfst-summarise
endif
if WANT_TAIL
MAYBE_TAIL=hfst-tail
endif
if WANT_TXT2FST
MAYBE_TXT2FST=hfst-txt2fst
endif
if WANT_CALCULATE
MAYBE_CALCULATE=hfst-calculate
MAYBE_ALIASES+=hfst-sfstpl2fst
endif
if WANT_SUBSTITUTE
MAYBE_SUBSTITUTE=hfst-substitute
endif
if WANT_FLAG_DIACRITICS
MAYBE_FLAG_DIACRITICS=hfst-flag-diacritics
endif
if WANT_XFST_COMPILER
MAYBE_XFST_COMPILER=hfst-xfst2fst
endif

noinst_HEADERS= \
	calculate_functions.h hfst-commandline.h hfst-program-options.h \
	hfst-compiler.h HfstCompiler.h HfstBasic.h HfstAlphabet.h HfstUtf8.h parsers/XreCompiler.h \
	parsers/xre_utils.h \
	inc/check-params-binary.h inc/check-params-common.h \
	inc/check-params-unary.h  inc/getopt-cases-binary.h \
	inc/getopt-cases-common.h inc/getopt-cases-error.h  \
	inc/getopt-cases-unary.h  inc/globals-binary.h      \
	inc/globals-common.h      inc/globals-unary.h \
	HfstStrings2FstTokenizer.h HfstLookupFlagDiacritics.h

# sort alphabetically
bin_PROGRAMS= \
			 $(MAYBE_COMPARE) $(MAYBE_COMPOSE) $(MAYBE_CONCATENATE)     \
			 $(MAYBE_CONJUNCT) $(MAYBE_DETERMINIZE) $(MAYBE_DISJUNCT)   \
			 $(MAYBE_FORMAT) $(MAYBE_FST2FST) $(MAYBE_FST2STRINGS)      \
			 $(MAYBE_FST2TXT) $(MAYBE_HEAD) $(MAYBE_INVERT)             \
			 $(MAYBE_LOOKUP) $(MAYBE_MINIMIZE) $(MAYBE_NAME)            \
			 $(MAYBE_PROJECT) $(MAYBE_PREPROCESS_FOR_OPTIMIZED_LOOKUP)  \
			 $(MAYBE_PUSH_WEIGHTS) $(MAYBE_REGEXP2FST)                  \
			 $(MAYBE_REMOVE_EPSILONS) $(MAYBE_REPEAT) $(MAYBE_REVERSE)  \
			 $(MAYBE_SPLIT) $(MAYBE_STRINGS2FST) $(MAYBE_STRIP_HEADER)  \
			 $(MAYBE_SUBTRACT) $(MAYBE_SUMMARIZE) $(MAYBE_TAIL)         \
			 $(MAYBE_TXT2FST) $(MAYBE_CALCULATE) $(MAYBE_SUBSTITUTE)    \
			 $(MAYBE_FLAG_DIACRITICS) $(MAYBE_LEXC_COMPILER)            \
			 $(MAYBE_XFST_COMPILER) $(MAYBE_FORMAT)                     \
			 $(MAYBE_COMPOSE_INTERSECT) $(MAYBE_NAME) $(MAYBE_PAIR_TEST)

bin_SCRIPTS=$(MAYBE_ALIASES)

HFST_COMMON_SRC=hfst-program-options.cc hfst-commandline.cc
# sort alphabetically
hfst_calculate_SOURCES=hfst-scanner.ll hfst-compiler.yy HfstCompiler.cc HfstBasic.cc HfstAlphabet.cc HfstUtf8.cc $(HFST_COMMON_SRC)
hfst_compare_SOURCES=hfst-compare.cc $(HFST_COMMON_SRC)
hfst_compose_intersect_SOURCES=hfst-compose-intersect.cc $(HFST_COMMON_SRC)
hfst_compose_SOURCES=hfst-compose.cc $(HFST_COMMON_SRC)
hfst_concatenate_SOURCES=hfst-concatenate.cc $(HFST_COMMON_SRC)
hfst_conjunct_SOURCES=hfst-conjunct.cc $(HFST_COMMON_SRC)
hfst_determinize_SOURCES=hfst-determinize.cc $(HFST_COMMON_SRC)
hfst_disjunct_SOURCES=hfst-disjunct.cc $(HFST_COMMON_SRC)
hfst_flag_diacritics_SOURCES=hfst-flag-diacritics.cc $(HFST_COMMON_SRC)
hfst_format_SOURCES=hfst-format.cc $(HFST_COMMON_SRC)
hfst_fst2fst_SOURCES=hfst-fst2fst.cc $(HFST_COMMON_SRC)
hfst_fst2strings_SOURCES=hfst-fst2strings.cc $(HFST_COMMON_SRC)
hfst_fst2txt_SOURCES=hfst-fst2txt.cc $(HFST_COMMON_SRC)
hfst_head_SOURCES=hfst-head.cc $(HFST_COMMON_SRC)
hfst_invert_SOURCES=hfst-invert.cc $(HFST_COMMON_SRC)
hfst_lexc2fst_SOURCES=hfst-lexc-compiler.cc $(HFST_COMMON_SRC) \
					  parsers/LexcCompiler.cc parsers/lexc-parser.yy \
					  parsers/lexc-lexer.ll parsers/lexc-utils.cc \
					  parsers/XreCompiler.cc parsers/xre_parse.yy \
					  parsers/xre_lex.ll parsers/xre_utils.cc
hfst_lookup_SOURCES=hfst-lookup.cc $(HFST_COMMON_SRC) \
HfstStrings2FstTokenizer.cc HfstLookupFlagDiacritics.cc
hfst_pair_test_SOURCES=hfst-pair-test.cc $(HFST_COMMON_SRC) \
HfstStrings2FstTokenizer.cc 
hfst_minimize_SOURCES=hfst-minimize.cc $(HFST_COMMON_SRC)
hfst_name_SOURCES=hfst-name.cc $(HFST_COMMON_SRC)
hfst_preprocess_for_optimized_lookup_format_SOURCES=hfst-preprocess-for-optimized-lookup-format.cc\
													 $(HFST_COMMON_SRC)
hfst_project_SOURCES=hfst-project.cc $(HFST_COMMON_SRC)
hfst_push_weights_SOURCES=hfst-push-weights.cc $(HFST_COMMON_SRC)
hfst_regexp2fst_SOURCES=hfst-regexp2fst.cc $(HFST_COMMON_SRC) \
						parsers/XreCompiler.cc parsers/xre_parse.yy \
						parsers/xre_lex.ll parsers/xre_utils.cc
hfst_remove_epsilons_SOURCES=hfst-remove-epsilons.cc $(HFST_COMMON_SRC)
hfst_repeat_SOURCES=hfst-repeat.cc $(HFST_COMMON_SRC)
hfst_reverse_SOURCES=hfst-reverse.cc $(HFST_COMMON_SRC)
hfst_split_SOURCES=hfst-split.cc $(HFST_COMMON_SRC)
hfst_strings2fst_SOURCES=hfst-strings2fst.cc $(HFST_COMMON_SRC) \
HfstStrings2FstTokenizer.cc
hfst_strip_header_SOURCES=hfst-strip-header.cc $(HFST_COMMON_SRC)
hfst_substitute_SOURCES=hfst-substitute.cc $(HFST_COMMON_SRC)
hfst_subtract_SOURCES=hfst-subtract.cc $(HFST_COMMON_SRC)
hfst_summarize_SOURCES=hfst-summarize.cc $(HFST_COMMON_SRC)
hfst_tail_SOURCES=hfst-tail.cc $(HFST_COMMON_SRC)
hfst_txt2fst_SOURCES=hfst-txt2fst.cc $(HFST_COMMON_SRC)
hfst_xfst2fst_SOURCES=hfst-xfst-compiler.cc $(HFST_COMMON_SRC) \
					  parsers/XfstCompiler.cc parsers/xfst-parser.yy \
					  parsers/xfst-lexer.ll parsers/xfst-utils.cc \
					  parsers/LexcCompiler.cc parsers/lexc-parser.yy \
					  parsers/lexc-lexer.ll parsers/lexc-utils.cc \
					  parsers/XreCompiler.cc parsers/xre_parse.yy \
					  parsers/xre_lex.ll parsers/xre_utils.cc

# aliases:
hfst-determinise$(EXEEXT): hfst-determinize$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-expand$(EXEEXT): hfst-fst2strings$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-intersect$(EXEEXT): hfst-conjunct$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-minimise$(EXEEXT): hfst-minimize$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-minus$(EXEEXT): hfst-subtract$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-optimized-lookup$(EXEEXT): hfst-lookup$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-optimised-lookup$(EXEEXT): hfst-lookup$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-preprocess-for-optimised-lookup-format$(EXEEXT): hfst-preprocess-for-optimized-lookup-format$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-sfstpl2fst$(EXEEXT): hfst-calculate$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-summarise$(EXEEXT): hfst-summarize$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

hfst-union$(EXEEXT): hfst-disjunct$(EXEEXT)
	$(AM_V_GEN)$(LN_S) $(notdir $<) $@

BUILT_SOURCES=hfst-scanner.h hfst-compiler.h
# make make realise headers automatically follow from flexing
hfst-scanner.h: hfst-scanner.cc

hfst_calculate_CXXFLAGS= -Wall -Wno-error

## YACC and FLEX with autotools argh
AM_YFLAGS=-d
AM_LFLAGS=--outfile=$(LEX_OUTPUT_ROOT).c 

