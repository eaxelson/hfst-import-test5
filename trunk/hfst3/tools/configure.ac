## Process this file with autoconf to produce configure script

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

AC_PREREQ([2.60])
AC_INIT([hfst-tools], [3.0], [hfst-bugs@helsinki.fi])
AC_CONFIG_AUX_DIR([config-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/hfst-subtract.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall foreign std-options])
AC_REVISION([$Revision: 1.9 $])

# if flex 3.X ever comes out, this breaks :)
AC_PROG_LEX
LEX_VERSION=`$LEX --version`
if test -z "`echo "$LEX_VERSION" | grep 'flex 2\.[[5-9]]\.[[3-9][4-9]]'`"
then
	AC_MSG_WARN([
!! Building hfst successfully requires flex newer than
!! 2.5.33 on some platforms. Unless you're building with
!! pre-flex-generated sources, building will probably fail.])
fi
# ' (because syntax highlight is being silly)

ifdef([LT_INIT], [LT_INIT], [AC_PROG_LIBTOOL])

# allow programs to be built separately
AC_ARG_ENABLE([compare],
			  [AS_HELP_STRING([--enable-compare],
							  [build compare tool @<:@default=no@:>@])],
			  [enable_compare=$enableval],
			  [enable_compare=no])
AM_CONDITIONAL([WANT_COMPARE], [test x$enable_compare != xno])
AC_ARG_ENABLE([compose],
			  [AS_HELP_STRING([--enable-compose],
							  [build compose tool @<:@default=no@:>@])],
			  [enable_compose=$enableval],
			  [enable_compose=no])
AM_CONDITIONAL([WANT_COMPOSE], [test x$enable_compose != xno])
AC_ARG_ENABLE([compose_intersect],
			  [AS_HELP_STRING([--enable-compose-intersect],
							  [build compose-intersect tool @<:@default=no@:>@])],
			  [enable_compose_intersect=$enableval],
			  [enable_compose_intersect=no])
AM_CONDITIONAL([WANT_COMPOSE_INTERSECT], [test x$enable_compose_intersect != xno])
AC_ARG_ENABLE([concatenate],
			  [AS_HELP_STRING([--enable-concatenate],
							  [build concatenate tool @<:@default=yes@:>@])],
			  [enable_concatenate=$enableval],
			  [enable_concatenate=yes])
AM_CONDITIONAL([WANT_CONCATENATE], [test x$enable_concatenate != xno])
AC_ARG_ENABLE([conjunct],
			  [AS_HELP_STRING([--enable-conjunct],
							  [build conjunct tool @<:@default=no@:>@])],
			  [enable_conjunct=$enableval],
			  [enable_conjunct=no])
AM_CONDITIONAL([WANT_CONJUNCT], [test x$enable_conjunct != xno])
AC_ARG_ENABLE([determinize],
			  [AS_HELP_STRING([--enable-determinize],
							  [build determinize tool @<:@default=no@:>@])],
			  [enable_determinize=$enableval],
			  [enable_determinize=no])
AM_CONDITIONAL([WANT_DETERMINIZE], [test x$enable_determinize != xno])
AC_ARG_ENABLE([diff_test],
			  [AS_HELP_STRING([--enable-diff-test],
							  [build diff-test tool @<:@default=no@:>@])],
			  [enable_diff_test=$enableval],
			  [enable_diff_test=no])
AM_CONDITIONAL([WANT_DIFF_TEST], [test x$enable_diff_test != xno])
AC_ARG_ENABLE([disjunct],
			  [AS_HELP_STRING([--enable-disjunct],
							  [build disjunct tool @<:@default=no@:>@])],
			  [enable_disjunct=$enableval],
			  [enable_disjunct=no])
AM_CONDITIONAL([WANT_DISJUNCT], [test x$enable_disjunct != xno])
AC_ARG_ENABLE([format],
			  [AS_HELP_STRING([--enable-format],
							  [build format tool @<:@default=no@:>@])],
			  [enable_format=$enableval],
			  [enable_format=no])
AM_CONDITIONAL([WANT_FORMAT], [test x$enable_format != xno])
AC_ARG_ENABLE([fst2fst],
			  [AS_HELP_STRING([--enable-fst2fst],
							  [build fst2fst tool @<:@default=no@:>@])],
			  [enable_fst2fst=$enableval],
			  [enable_fst2fst=no])
AM_CONDITIONAL([WANT_FST2FST], [test x$enable_fst2fst != xno])
AC_ARG_ENABLE([fst2pairstrings],
			  [AS_HELP_STRING([--enable-fst2pairstrings],
							  [build fst2pairstrings tool @<:@default=no@:>@])],
			  [enable_fst2pairstrings=$enableval],
			  [enable_fst2pairstrings=no])
AM_CONDITIONAL([WANT_FST2PAIRSTRINGS], [test x$enable_fst2pairstrings != xno])
AC_ARG_ENABLE([fst2strings],
			  [AS_HELP_STRING([--enable-fst2strings],
							  [build fst2strings tool @<:@default=no@:>@])],
			  [enable_fst2strings=$enableval],
			  [enable_fst2strings=no])
AM_CONDITIONAL([WANT_FST2STRINGS], [test x$enable_fst2strings != xno])
AC_ARG_ENABLE([fst2txt],
			  [AS_HELP_STRING([--enable-fst2txt],
							  [build fst2txt tool @<:@default=no@:>@])],
			  [enable_fst2txt=$enableval],
			  [enable_fst2txt=no])
AM_CONDITIONAL([WANT_FST2TXT], [test x$enable_fst2txt != xno])
AC_ARG_ENABLE([head],
			  [AS_HELP_STRING([--enable-head],
							  [build head tool @<:@default=no@:>@])],
			  [enable_head=$enableval],
			  [enable_head=no])
AM_CONDITIONAL([WANT_HEAD], [test x$enable_head != xno])
AC_ARG_ENABLE([invert],
			  [AS_HELP_STRING([--enable-invert],
							  [build invert tool @<:@default=no@:>@])],
			  [enable_invert=$enableval],
			  [enable_invert=no])
AM_CONDITIONAL([WANT_INVERT], [test x$enable_invert != xno])
AC_ARG_ENABLE([lookup],
			  [AS_HELP_STRING([--enable-lookup],
							  [build lookup tool @<:@default=no@:>@])],
			  [enable_lookup=$enableval],
			  [enable_lookup=no])
AM_CONDITIONAL([WANT_LOOKUP], [test x$enable_lookup != xno])
AC_ARG_ENABLE([lookup_optimize],
			  [AS_HELP_STRING([--enable-lookup-optimize],
							  [build lookup-optimize tool @<:@default=no@:>@])],
			  [enable_lookup_optimize=$enableval],
			  [enable_lookup_optimize=no])
AM_CONDITIONAL([WANT_LOOKUP_OPTIMIZE], [test x$enable_lookup_optimize != xno])
AC_ARG_ENABLE([minimize],
			  [AS_HELP_STRING([--enable-minimize],
							  [build minimize tool @<:@default=no@:>@])],
			  [enable_minimize=$enableval],
			  [enable_minimize=no])
AM_CONDITIONAL([WANT_MINIMIZE], [test x$enable_minimize != xno])
AC_ARG_ENABLE([pair_test],
			  [AS_HELP_STRING([--enable-pair-test],
							  [build pair-test tool @<:@default=no@:>@])],
			  [enable_pair_test=$enableval],
			  [enable_pair_test=no])
AM_CONDITIONAL([WANT_PAIR_TEST], [test x$enable_pair_test != xno])
AC_ARG_ENABLE([project],
			  [AS_HELP_STRING([--enable-project],
							  [build project tool @<:@default=no@:>@])],
			  [enable_project=$enableval],
			  [enable_project=no])
AM_CONDITIONAL([WANT_PROJECT], [test x$enable_project != xno])
AC_ARG_ENABLE([push_weights],
			  [AS_HELP_STRING([--enable-push-weights],
							  [build push-weights tool @<:@default=no@:>@])],
			  [enable_push_weights=$enableval],
			  [enable_push_weights=no])
AM_CONDITIONAL([WANT_PUSH_WEIGHTS], [test x$enable_push_weights != xno])
AC_ARG_ENABLE([regexp2fst],
			  [AS_HELP_STRING([--enable-regexp2fst],
							  [build regexp2fst tool @<:@default=no@:>@])],
			  [enable_regexp2fst=$enableval],
			  [enable_regexp2fst=no])
AM_CONDITIONAL([WANT_REGEXP2FST], [test x$enable_regexp2fst != xno])
AC_ARG_ENABLE([remove_epsilons],
			  [AS_HELP_STRING([--enable-remove-epsilons],
							  [build remove-epsilons tool @<:@default=no@:>@])],
			  [enable_remove_epsilons=$enableval],
			  [enable_remove_epsilons=no])
AM_CONDITIONAL([WANT_REMOVE_EPSILONS], [test x$enable_remove_epsilons != xno])
AC_ARG_ENABLE([repeat],
			  [AS_HELP_STRING([--enable-repeat],
							  [build repeat tool @<:@default=no@:>@])],
			  [enable_repeat=$enableval],
			  [enable_repeat=no])
AM_CONDITIONAL([WANT_REPEAT], [test x$enable_repeat != xno])
AC_ARG_ENABLE([reverse],
			  [AS_HELP_STRING([--enable-reverse],
							  [build reverse tool @<:@default=no@:>@])],
			  [enable_reverse=$enableval],
			  [enable_reverse=no])
AM_CONDITIONAL([WANT_REVERSE], [test x$enable_reverse != xno])
AC_ARG_ENABLE([split],
			  [AS_HELP_STRING([--enable-split],
							  [build split tool @<:@default=no@:>@])],
			  [enable_split=$enableval],
			  [enable_split=no])
AM_CONDITIONAL([WANT_SPLIT], [test x$enable_split != xno])
AC_ARG_ENABLE([strings2fst],
			  [AS_HELP_STRING([--enable-strings2fst],
							  [build strings2fst tool @<:@default=no@:>@])],
			  [enable_strings2fst=$enableval],
			  [enable_strings2fst=no])
AM_CONDITIONAL([WANT_STRINGS2FST], [test x$enable_strings2fst != xno])
AC_ARG_ENABLE([subtract],
			  [AS_HELP_STRING([--enable-subtract],
							  [build subtract tool @<:@default=no@:>@])],
			  [enable_subtract=$enableval],
			  [enable_subtract=no])
AM_CONDITIONAL([WANT_SUBTRACT], [test x$enable_subtract != xno])
AC_ARG_ENABLE([summarize],
			  [AS_HELP_STRING([--enable-summarize],
							  [build summarize tool @<:@default=no@:>@])],
			  [enable_summarize=$enableval],
			  [enable_summarize=no])
AM_CONDITIONAL([WANT_SUMMARIZE], [test x$enable_summarize != xno])
AC_ARG_ENABLE([symbols],
			  [AS_HELP_STRING([--enable-symbols],
							  [build symbols tool @<:@default=no@:>@])],
			  [enable_symbols=$enableval],
			  [enable_symbols=no])
AM_CONDITIONAL([WANT_SYMBOLS], [test x$enable_symbols != xno])
AC_ARG_ENABLE([tail],
			  [AS_HELP_STRING([--enable-tail],
							  [build tail tool @<:@default=no@:>@])],
			  [enable_tail=$enableval],
			  [enable_tail=no])
AM_CONDITIONAL([WANT_TAIL], [test x$enable_tail != xno])
AC_ARG_ENABLE([txt2fst],
			  [AS_HELP_STRING([--enable-txt2fst],
							  [build txt2fst tool @<:@default=no@:>@])],
			  [enable_txt2fst=$enableval],
			  [enable_txt2fst=no])
AM_CONDITIONAL([WANT_TXT2FST], [test x$enable_txt2fst != xno])
AC_ARG_ENABLE([calculate],
			  [AS_HELP_STRING([--enable-calculate],
							  [build calculate tool @<:@default=no@:>@])],
			  [enable_calculate=$enableval],
			  [enable_calculate=no])
AM_CONDITIONAL([WANT_CALCULATE], [test x$enable_calculate != xno])
AC_ARG_ENABLE([optimized-lookup],
			  [AS_HELP_STRING([--enable-optimized-lookup],
							  [build optimized lookup tool @<:@default=no@:>@])],
			  [enable_optimized_lookup=$enableval],
			  [enable_optimized_lookup=no])
AM_CONDITIONAL([WANT_OPTIMIZED_LOOKUP], [test x$enable_optimized_lookup != xno])
AC_ARG_ENABLE([substitute],
			  [AS_HELP_STRING([--enable-substitute],
							  [build substitution tool @<:@default=no@:>@])],
			  [enable_substitute=$enableval],
			  [enable_substitute=no])
AM_CONDITIONAL([WANT_SUBSTITUTE], [test x$enable_substitute != xno])
AC_ARG_ENABLE([flag_diacritics],
			  [AS_HELP_STRING([--enable-flag-diacritics],
							  [build flag diacritic tool @<:@default=no@:>@])],
			  [enable_flag_diacritics=$enableval],
			  [enable_flag_diacritics=no])
AM_CONDITIONAL([WANT_FLAG_DIACRITICS], [test x$enable_flag_diacritics != xno])
AC_ARG_ENABLE([omor_evaluate],
			  [AS_HELP_STRING([--enable-omor-evaluate],
							  [build omor evaluation tool @<:@default=no@:>@])],
			  [enable_omor_evaluate=$enableval],
			  [enable_omor_evaluate=no])
AM_CONDITIONAL([WANT_OMOR_EVALUATE], [test x$enable_omor_evaluate != xno])


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_YACC

# Checks for libraries.
AC_LANG([C++])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h unistd.h])
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([backward/hash_set ext/hash_set hash_set])
AC_CHECK_HEADERS([backward/hash_map ext/hash_map hash_map])
AC_LANG_POP
AC_HEADER_ASSERT

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor strchr strdup strerror strncasecmp strndup getline])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
## keep the list alphabetically sorted (vim; !sort)
cat <<EOF
-- Building $PACKAGE v$VERSION:
    * calculate: $enable_calculate
    * compare: $enable_compare
    * compose: $enable_compose
    * compose-intersect: $enable_compose_intersect
    * concatenate: $enable_concatenate
    * conjunct: $enable_conjunct
    * determinize: $enable_determinize
    * diff-test: $enable_diff_test
    * disjunct: $enable_disjunct
    * flag-diacritics: $enable_flag_diacritics
    * format: $enable_format
    * fst2fst: $enable_fst2fst
    * fst2pairstrings: $enable_fst2pairstrings
    * fst2strings: $enable_fst2strings
    * fst2txt: $enable_fst2txt
    * head: $enable_head
    * invert: $enable_invert
    * lookup: $enable_lookup
    * lookup-optimize: $enable_lookup_optimize
    * minimize: $enable_minimize
    * omor-evaluate: $enable_omor_evaluate
    * optimized-lookup: $enable_optimized_lookup
    * pair-test: $enable_pair_test
    * project: $enable_project
    * push-weights: $enable_push_weights
    * regexp2fst: $enable_regexp2fst
    * remove-epsilons: $enable_remove_epsilons
    * repeat: $enable_repeat
    * reverse: $enable_reverse
    * split: $enable_split
    * strings2fst: $enable_strings2fst
    * substitute: $enable_substitute
    * subtract: $enable_subtract
    * summarize: $enable_summarize
    * symbols: $enable_symbols
    * tail: $enable_tail
    * txt2fst: $enable_txt2fst
EOF

