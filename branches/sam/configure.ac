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

# Information on package
HFST_NAME=hfst
HFST_MAJOR=3
HFST_MINOR=3
HFST_EXTENSION=11
HFST_VERSION=$HFST_MAJOR.$HFST_MINOR.$HFST_EXTENSION
# for package-config pc file
LIBHFST_NAME=hfst
LIBHFST_MAJOR=3
LIBHFST_MINOR=3
LIBHFST_EXTENSION=11
LIBHFST_VERSION=$LIBHFST_MAJOR.$LIBHFST_MINOR.$LIBHFST_EXTENSION

AC_INIT([hfst], [3.3.11], [hfst-bugs@helsinki.fi], [hfst], [http://hfst.sf.net])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror std-options foreign check-news])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([libhfst/src/HfstTransducer.h])
AC_CONFIG_HEADERS([config.h])

AC_SUBST(LIBHFST_MAJOR)
AC_SUBST(LIBHFST_MINOR)
AC_SUBST(LIBHFST_EXTENSION)
AC_SUBST(LIBHFST_VERSION)
AC_SUBST(LIBHFST_NAME)

# allow backends to be removed
AC_ARG_WITH([sfst],
            [AS_HELP_STRING([--with-sfst],
                            [process unweighted fsts with SFST @<:@default=no@:>@])],
            [],
            [with_sfst=no])
AS_IF([test "x$with_sfst" != xno], [AC_DEFINE([HAVE_SFST], [1],
                                              [Define to compile SFST support in HFST])])
AM_CONDITIONAL([WANT_SFST], [test x$with_sfst != xno])
AC_ARG_WITH([openfst],
            [AS_HELP_STRING([--with-openfst],
                            [process weighted fsts with OpenFst @<:@default=yes@:>@])],
            [],
            [with_openfst=yes])
AS_IF([test "x$with_openfst" != xno], [AC_DEFINE([HAVE_OPENFST], [1],
                                              [Define to compile OpenFst support in HFST])])
AM_CONDITIONAL([WANT_OPENFST], [test x$with_openfst != xno])
AC_ARG_WITH([foma],
            [AS_HELP_STRING([--with-foma],
                            [process unweighted fsts with foma @<:@default=no@:>@])],
            [],
            [with_foma=no])
AS_IF([test "x$with_foma" != xno], [AC_DEFINE([HAVE_FOMA], [1],
                                              [Define to compile foma support in HFST])])
AM_CONDITIONAL([WANT_FOMA], [test x$with_foma != xno])

### Add here your library ###
#AC_ARG_WITH([my_transducer_library],
#            [AS_HELP_STRING([--with-my-transducer-library],
#                            [process unweighted fsts with my transducer library @<:@default=no@:>@])],
#            [],
#            [with_my_transducer_library=no])
#AS_IF([test "x$with_my_transducer_library" != xno], [AC_DEFINE([HAVE_MY_TRANSDUCER_LIBRARY], [1],
#                                              [Define to compile my transducer library support in HFST])])
#AM_CONDITIONAL([WANT_MY_TRANSDUCER_LIBRARY], [test x$with_my_transducer_library != xno])

AC_ARG_WITH([hfstol],
            [AS_HELP_STRING([--with-hfstol],
                            [run fsts with hfst optimized lookup library @<:@default=bundled@:>@])],
            [],
            [with_hfstol=bundled])
AS_IF([test "x$with_hfstol" != xno], [AC_DEFINE([HAVE_HFSTOL], [1],
                                              [Define to compile optimized lookup support in HFST])])
AM_CONDITIONAL([WANT_HFSTOL], [test x$with_hfstol != xno])

AS_IF([test "x$with_sfst" == "xno" -a "x$with_openfst" == "xno" -a "x$with_foma" == "xno" -a "x$with_hfstol" == "xno"], [AC_MSG_ERROR([must build with at least one fst backend])])

# relay unicode support to which backend library
AC_ARG_WITH([unicode_handler],
            [AS_HELP_STRING([--with-unicode-handler=backend],
                            [select library to use for unicode handling @<:@default=hfst@:>@])],
            [with_unicode_handler=$withval],
            [with_unicode_handler=hfst])
# make readline optional since mac os x has sucky implementation of readline
AC_ARG_WITH([readline],
            [AS_HELP_STRING([--with-readline],
                            [use readline for interactive programs @<:@default=no@:>@])],
            [with_readline=$withval],
            [with_readline=no])

# allow programs to be built separately (sort alphabetically)
AC_ARG_ENABLE([affix_guessify],
			  [AS_HELP_STRING([--enable-affix_guessify],
							  [build affix_guessify tool @<:@default=yes@:>@])],
			  [enable_affix_guessify=$enableval],
			  [enable_affix_guessify=yes])
AM_CONDITIONAL([WANT_AFFIX_GUESSIFY], [test x$enable_affix_guessify != xno])
AC_ARG_ENABLE([calculate],
			  [AS_HELP_STRING([--enable-calculate],
							  [build calculate tool @<:@default=no@:>@])],
			  [enable_calculate=$enableval],
			  [enable_calculate=no])
AM_CONDITIONAL([WANT_CALCULATE], [test x$enable_calculate != xno])
AC_ARG_ENABLE([compare],
			  [AS_HELP_STRING([--enable-compare],
							  [build compare tool @<:@default=yes@:>@])],
			  [enable_compare=$enableval],
			  [enable_compare=yes])
AM_CONDITIONAL([WANT_COMPARE], [test x$enable_compare != xno])
AC_ARG_ENABLE([compose],
			  [AS_HELP_STRING([--enable-compose],
							  [build compose tool @<:@default=yes@:>@])],
			  [enable_compose=$enableval],
			  [enable_compose=yes])
AM_CONDITIONAL([WANT_COMPOSE], [test x$enable_compose != xno])
AC_ARG_ENABLE([compose_intersect],
			  [AS_HELP_STRING([--enable-compose-intersect],
							  [build compose-intersect tool @<:@default=yes@:>@])],
			  [enable_compose_intersect=$enableval],
			  [enable_compose_intersect=yes])
AM_CONDITIONAL([WANT_COMPOSE_INTERSECT], [test x$enable_compose_intersect != xno])
AC_ARG_ENABLE([concatenate],
			  [AS_HELP_STRING([--enable-concatenate],
							  [build concatenate tool @<:@default=yes@:>@])],
			  [enable_concatenate=$enableval],
			  [enable_concatenate=yes])
AM_CONDITIONAL([WANT_CONCATENATE], [test x$enable_concatenate != xno])
AC_ARG_ENABLE([conjunct],
			  [AS_HELP_STRING([--enable-conjunct],
							  [build conjunct tool @<:@default=yes@:>@])],
			  [enable_conjunct=$enableval],
			  [enable_conjunct=yes])
AM_CONDITIONAL([WANT_CONJUNCT], [test x$enable_conjunct != xno])
AC_ARG_ENABLE([duplicate],
			  [AS_HELP_STRING([--enable-duplicate],
							  [build duplicate tool @<:@default=yes@:>@])],
			  [enable_duplicate=$enableval],
			  [enable_duplicate=yes])
AM_CONDITIONAL([WANT_DUPLICATE], [test x$enable_duplicate != xno])
AC_ARG_ENABLE([determinize],
			  [AS_HELP_STRING([--enable-determinize],
							  [build determinize tool @<:@default=yes@:>@])],
			  [enable_determinize=$enableval],
			  [enable_determinize=yes])
AM_CONDITIONAL([WANT_DETERMINIZE], [test x$enable_determinize != xno])
AC_ARG_ENABLE([disjunct],
			  [AS_HELP_STRING([--enable-disjunct],
							  [build disjunct tool @<:@default=yes@:>@])],
			  [enable_disjunct=$enableval],
			  [enable_disjunct=yes])
AM_CONDITIONAL([WANT_DISJUNCT], [test x$enable_disjunct != xno])
AC_ARG_ENABLE([edit_metadata],
			  [AS_HELP_STRING([--enable-edit-metadata],
							  [build metadata tool @<:@default=yes@:>@])],
			  [enable_edit_metadata=$enableval],
			  [enable_edit_metadata=yes])
AM_CONDITIONAL([WANT_EDIT_METADATA], [test x$enable_edit_metadata != xno])
AC_ARG_ENABLE([format],
			  [AS_HELP_STRING([--enable-format],
							  [build format tool @<:@default=yes@:>@])],
			  [enable_format=$enableval],
			  [enable_format=yes])
AM_CONDITIONAL([WANT_FORMAT], [test x$enable_format != xno])
AC_ARG_ENABLE([fst2fst],
			  [AS_HELP_STRING([--enable-fst2fst],
							  [build fst2fst tool @<:@default=yes@:>@])],
			  [enable_fst2fst=$enableval],
			  [enable_fst2fst=yes])
AM_CONDITIONAL([WANT_FST2FST], [test x$enable_fst2fst != xno])
AC_ARG_ENABLE([fst2strings],
			  [AS_HELP_STRING([--enable-fst2strings],
							  [build fst2strings tool @<:@default=yes@:>@])],
			  [enable_fst2strings=$enableval],
			  [enable_fst2strings=yes])
AM_CONDITIONAL([WANT_FST2STRINGS], [test x$enable_fst2strings != xno])
AC_ARG_ENABLE([fst2txt],
			  [AS_HELP_STRING([--enable-fst2txt],
							  [build fst2txt tool @<:@default=yes@:>@])],
			  [enable_fst2txt=$enableval],
			  [enable_fst2txt=yes])
AM_CONDITIONAL([WANT_FST2TXT], [test x$enable_fst2txt != xno])
AC_ARG_ENABLE([grep],
			  [AS_HELP_STRING([--enable-grep],
							  [build grep tool @<:@default=yes@:>@])],
			  [enable_grep=$enableval],
			  [enable_grep=yes])
AM_CONDITIONAL([WANT_GREP], [test x$enable_grep != xno])
AC_ARG_ENABLE([head],
			  [AS_HELP_STRING([--enable-head],
							  [build head tool @<:@default=yes@:>@])],
			  [enable_head=$enableval],
			  [enable_head=yes])
AM_CONDITIONAL([WANT_HEAD], [test x$enable_head != xno])
AC_ARG_ENABLE([invert],
			  [AS_HELP_STRING([--enable-invert],
							  [build invert tool @<:@default=yes@:>@])],
			  [enable_invert=$enableval],
			  [enable_invert=yes])
AM_CONDITIONAL([WANT_INVERT], [test x$enable_invert != xno])
AC_ARG_ENABLE([lexc],
              [AS_HELP_STRING([--enable-lexc],
                              [build lexc cli and readline @<:@default=no@:>@])],
              [enable_lexc=$enableval],
              [enable_lexc=no])
AM_CONDITIONAL([WANT_LEXC], [test "x$enable_lexc" != xno])
AC_ARG_ENABLE([twolc],
              [AS_HELP_STRING([--enable-twolc],
                              [build twolc @<:@default=yes@:>@])],
              [enable_twolc=$enableval],
              [enable_twolc=yes])
AM_CONDITIONAL([WANT_TWOLC], [test "x$enable_twolc" != xno])
AC_ARG_ENABLE([tagger],
              [AS_HELP_STRING([--enable-tagger],
                              [build tagger utilities @<:@default=no@:>@])],
              [enable_tagger=$enableval],
              [enable_tagger=no])
AM_CONDITIONAL([WANT_TAGGER], [test "x$enable_tagger" != xno])
AC_ARG_ENABLE([lexc2fst],
              [AS_HELP_STRING([--enable-lexc2fst],
                              [build lexc2fst cli @<:@default=yes@:>@])],
              [enable_lexc2fst=$enableval],
              [enable_lexc2fst=yes])
AM_CONDITIONAL([WANT_LEXC2FST], [test "x$enable_lexc2fst" != xno])
AC_ARG_ENABLE([lookup],
			  [AS_HELP_STRING([--enable-lookup],
							  [build lookup tool @<:@default=yes@:>@])],
			  [enable_lookup=$enableval],
			  [enable_lookup=yes])
AM_CONDITIONAL([WANT_LOOKUP], [test x$enable_lookup != xno])
AC_ARG_ENABLE([pair-test],
			  [AS_HELP_STRING([--enable-pair-test],
							  [build pair-test tool @<:@default=yes@:>@])],
			  [enable_pair_test=$enableval],
			  [enable_pair_test=yes])
AM_CONDITIONAL([WANT_PAIR_TEST], [test x$enable_pair_test != xno])
AC_ARG_ENABLE([minimize],
			  [AS_HELP_STRING([--enable-minimize],
							  [build minimize tool @<:@default=yes@:>@])],
			  [enable_minimize=$enableval],
			  [enable_minimize=yes])
AM_CONDITIONAL([WANT_MINIMIZE], [test x$enable_minimize != xno])
AC_ARG_ENABLE([name],
			  [AS_HELP_STRING([--enable-name],
							  [build name tool @<:@default=yes@:>@])],
			  [enable_name=$enableval],
			  [enable_name=yes])
AM_CONDITIONAL([WANT_NAME], [test x$enable_name != xno])
AC_ARG_ENABLE([optimized_lookup],
			  [AS_HELP_STRING([--enable-optimized-lookup],
							  [build optimized-lookup tool @<:@default=yes@:>@])],
			  [enable_optimized_lookup=$enableval],
			  [enable_optimized_lookup=yes])
AM_CONDITIONAL([WANT_OPTIMIZED_LOOKUP], [test x$enable_optimized_lookup != xno])
AC_ARG_ENABLE([preprocess_for_optimized_lookup],
			  [AS_HELP_STRING([--enable-remove-epsilons],
							  [build remove-epsilons tool @<:@default=yes@:>@])],
			  [enable_preprocess_for_optimized_lookup=$enableval],
			  [enable_preprocess_for_optimized_lookup=yes])
AM_CONDITIONAL([WANT_PREPROCESS_FOR_OPTIMIZED_LOOKUP], [test x$enable_preprocess_for_optimized_lookup != xno])
AC_ARG_ENABLE([project],
			  [AS_HELP_STRING([--enable-project],
							  [build project tool @<:@default=yes@:>@])],
			  [enable_project=$enableval],
			  [enable_project=yes])
AM_CONDITIONAL([WANT_PROJECT], [test x$enable_project != xno])
AC_ARG_ENABLE([proc],
			  [AS_HELP_STRING([--enable-proc],
							  [build text processing tool @<:@default=no@:>@])],
			  [enable_proc=$enableval],
			  [enable_proc=yes])
AM_CONDITIONAL([WANT_PROC], [test x$enable_proc != xno])
AC_ARG_ENABLE([push_weights],
			  [AS_HELP_STRING([--enable-push-weights],
							  [build push-weights tool @<:@default=yes@:>@])],
			  [enable_push_weights=$enableval],
			  [enable_push_weights=yes])
AM_CONDITIONAL([WANT_PUSH_WEIGHTS], [test x$enable_push_weights != xno])
AC_ARG_ENABLE([regexp2fst],
			  [AS_HELP_STRING([--enable-regexp2fst],
							  [build regexp2fst tool @<:@default=yes@:>@])],
			  [enable_regexp2fst=$enableval],
			  [enable_regexp2fst=yes])
AM_CONDITIONAL([WANT_REGEXP2FST], [test x$enable_regexp2fst != xno])
AC_ARG_ENABLE([pmatch2fst],
			  [AS_HELP_STRING([--enable-pmatch2fst],
							  [build pmatch2fst tool @<:@default=yes@:>@])],
			  [enable_pmatch2fst=$enableval],
			  [enable_pmatch2fst=yes])
AM_CONDITIONAL([WANT_PMATCH2FST], [test x$enable_pmatch2fst != xno])
AC_ARG_ENABLE([remove_epsilons],
			  [AS_HELP_STRING([--enable-remove-epsilons],
							  [build remove-epsilons tool @<:@default=yes@:>@])],
			  [enable_remove_epsilons=$enableval],
			  [enable_remove_epsilons=yes])
AM_CONDITIONAL([WANT_REMOVE_EPSILONS], [test x$enable_remove_epsilons != xno])
AC_ARG_ENABLE([repeat],
			  [AS_HELP_STRING([--enable-repeat],
							  [build repeat tool @<:@default=yes@:>@])],
			  [enable_repeat=$enableval],
			  [enable_repeat=yes])
AM_CONDITIONAL([WANT_REPEAT], [test x$enable_repeat != xno])
AC_ARG_ENABLE([reverse],
			  [AS_HELP_STRING([--enable-reverse],
							  [build reverse tool @<:@default=yes@:>@])],
			  [enable_reverse=$enableval],
			  [enable_reverse=yes])
AM_CONDITIONAL([WANT_REVERSE], [test x$enable_reverse != xno])
AC_ARG_ENABLE([reweight],
			  [AS_HELP_STRING([--enable-reweight],
							  [build reweight tool @<:@default=yes@:>@])],
			  [enable_reweight=$enableval],
			  [enable_reweight=yes])
AM_CONDITIONAL([WANT_REWEIGHT], [test x$enable_reweight != xno])
AC_ARG_ENABLE([split],
			  [AS_HELP_STRING([--enable-split],
							  [build split tool @<:@default=yes@:>@])],
			  [enable_split=$enableval],
			  [enable_split=yes])
AM_CONDITIONAL([WANT_SPLIT], [test x$enable_split != xno])
AC_ARG_ENABLE([shuffle],
			  [AS_HELP_STRING([--enable-shuffle],
							  [build shuffle tool @<:@default=no@:>@])],
			  [enable_shuffle=$enableval],
			  [enable_shuffle=no])
AM_CONDITIONAL([WANT_SHUFFLE], [test x$enable_shuffle != xno])
AC_ARG_ENABLE([strings2fst],
			  [AS_HELP_STRING([--enable-strings2fst],
							  [build strings2fst tool @<:@default=yes@:>@])],
			  [enable_strings2fst=$enableval],
			  [enable_strings2fst=yes])
AM_CONDITIONAL([WANT_STRINGS2FST], [test x$enable_strings2fst != xno])
AC_ARG_ENABLE([strip_header],
			  [AS_HELP_STRING([--enable-strip-header],
							  [build strip-header tool @<:@default=yes@:>@])],
			  [enable_strip_header=$enableval],
			  [enable_strip_header=yes])
AM_CONDITIONAL([WANT_STRIP_HEADER], [test x$enable_strip_header != xno])
AC_ARG_ENABLE([substitute],
			  [AS_HELP_STRING([--enable-substitute],
							  [build substitution tool @<:@default=yes@:>@])],
			  [enable_substitute=$enableval],
			  [enable_substitute=yes])
AM_CONDITIONAL([WANT_SUBSTITUTE], [test x$enable_substitute != xno])
AC_ARG_ENABLE([subtract],
			  [AS_HELP_STRING([--enable-subtract],
							  [build subtract tool @<:@default=yes@:>@])],
			  [enable_subtract=$enableval],
			  [enable_subtract=yes])
AM_CONDITIONAL([WANT_SUBTRACT], [test x$enable_subtract != xno])
AC_ARG_ENABLE([summarize],
			  [AS_HELP_STRING([--enable-summarize],
							  [build summarize tool @<:@default=yes@:>@])],
			  [enable_summarize=$enableval],
			  [enable_summarize=yes])
AM_CONDITIONAL([WANT_SUMMARIZE], [test x$enable_summarize != xno])
AC_ARG_ENABLE([tail],
			  [AS_HELP_STRING([--enable-tail],
							  [build tail tool @<:@default=yes@:>@])],
			  [enable_tail=$enableval],
			  [enable_tail=yes])
AM_CONDITIONAL([WANT_TAIL], [test x$enable_tail != xno])
AC_ARG_ENABLE([traverse],
			  [AS_HELP_STRING([--enable-traverse],
							  [build traversal tool @<:@default=no@:>@])],
			  [enable_traverse=$enableval],
			  [enable_traverse=no])
AM_CONDITIONAL([WANT_TRAVERSE], [test x$enable_test != xno])
AC_ARG_ENABLE([test],
			  [AS_HELP_STRING([--enable-test],
							  [build test tool @<:@default=yes@:>@])],
			  [enable_test=$enableval],
			  [enable_test=yes])
AM_CONDITIONAL([WANT_TEST], [test x$enable_test != xno])
AC_ARG_ENABLE([txt2fst],
			  [AS_HELP_STRING([--enable-txt2fst],
							  [build txt2fst tool @<:@default=yes@:>@])],
			  [enable_txt2fst=$enableval],
			  [enable_txt2fst=yes])
AM_CONDITIONAL([WANT_TXT2FST], [test x$enable_txt2fst != xno])

# Disable stress tests by default
AC_ARG_ENABLE([stress-tests],
              [AS_HELP_STRING([--enable-stress-tests],
                              [include stress tests in check @<:@default=no@:>@])],
                              [enable_stress_tests=$enableval],
                              [enable_stress_tests=no])
AM_CONDITIONAL([WANT_STRESSTESTS], [test x$enable_stress_tests != xno])

# Checks for programs
AC_PROG_CC
AC_PROG_CXX
LT_INIT
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_YACC
AC_PROG_LEX
AC_PATH_PROG([FOMACLI], [foma], [false])
AC_PATH_PROG([GETOPT], [getopts], [false])
AM_CONDITIONAL([CAN_FOMAWRAP], [test x$FOMACLI != xfalse])
AM_PATH_PYTHON([2.4],[],[false])
# if flex 3.X ever comes out, this breaks :)
LEX_VERSION=`$LEX --version`
AS_IF([test -z "`echo "$LEX_VERSION" | grep 'flex 2\.[[5-9]]\.[[3-9][4-9]]'`"],
      [AC_MSG_WARN([Building hfst successfully requires flex newer than 2.5.33 on some platforms. Unless you are building with pre-flex-generated sources, building will probably fail.])])

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PATH_PROG([DOXYGEN], [doxygen], [no])
AM_CONDITIONAL([CAN_DOXYGEN], [test x$DOXYGEN != xno])

# Checks for libraries

AC_LANG_PUSH([C++])
AS_IF([test "x$with_openfst" != "xno"], 
      [AC_CHECK_LIB([pthread], [main])]
      [AC_CHECK_LIB([dl], [main])]
      [AC_CHECK_LIB([fst], [main], [],
                 [AC_MSG_FAILURE([openfst test failed (--without-openfst to disable)])])]
      [AC_CHECK_LIB([m], [main])])
AS_IF([test "x$with_sfst" != "xno"],
      [AC_CHECK_LIB([sfst], [main])]
      [AC_CHECK_LIB([sfst1], [main])])
AS_IF([test "x$ac_cv_lib_sfst_main" == xno -a "x$ac_cv_lib_sfst1_main" == xno],
      [AC_MSG_FAILURE([sfst tests failed (--without-sfst to disable)])])
AC_LANG_POP
AC_LANG_PUSH([C])
AS_IF([test "x$with_foma" != "xno"],
      [AC_CHECK_LIB([z], [main])]
      [AC_CHECK_LIB([foma], [main], [],
                    [AC_MSG_FAILURE([foma test failed (--without-foma to disable)])])])
AC_LANG_POP

AS_IF([test "x$with_unicode_handler" = "xglib"],
      [AC_CHECK_LIB([glib-2.0], [main])
       AC_DEFINE([USE_GLIB_UNICODE], 1, [if using glib for unicode string handling])])
AS_IF([test "x$with_unicode_handler" = "xicu"],
      [AC_MSG_FAILURE([ICU not yet implemented (--with-unicode-handler=hfst to disable)])])
AS_IF([test "x$with_readline" = "xyes"],
      [AC_SEARCH_LIBS([readline], [readline editline], 
                      [AC_DEFINE([HAVE_READLINE], 1, [if readline can be used])], 
                      [AC_MSG_FAILURE([readline test failed (--without-readline to disable)])])])

### Add here your library ###
#AS_IF([test "x$with_my_transducer_library" != "xno"],
#      [AC_CHECK_LIB([my_transducer_library], [main], [],
#                [AC_MSG_FAILURE([my transducer library test failed (--without-my-transducer-library to disable)])])])

# Checks for header files
AC_CHECK_HEADERS([limits.h stdlib.h string.h error.h glob.h locale.h langinfo.h])

AC_LANG_PUSH([C++])

AS_IF([test "x$with_sfst" != "xno"],
      [AC_CHECK_HEADERS([sfst/fst.h sfst1/fst.h])]
      [AC_CHECK_HEADERS([backward/hash_set ext/hash_set hash_set])]
      [AC_CHECK_HEADERS([backward/hash_map ext/hash_map hash_map])])
AS_IF([test "x$with_openfst" != "xno"],
      [AC_CHECK_HEADERS([fst/fstlib.h])])
AC_LANG_POP
AC_LANG_PUSH([C])
AS_IF([test "x$with_foma" != "xno"],
      [AC_CHECK_HEADERS([fomalib.h])])
AC_LANG_POP
AC_LANG_PUSH([C])
AS_IF([test "x$with_readline" = "xyes"],
      [AC_CHECK_HEADERS([readline/readline.h readline/history.h])])
AS_IF([test "x$with_unicode_handler" = "xglib"],
      [AC_PATH_PROG([PKG_CONFIG], [pkg-config])
       AS_IF([test "x$PKG_CONFIG" != "xno"],
             [AC_SUBST([GLIB_CPPFLAGS], [`$PKG_CONFIG glib-2.0 --cflags`])
              AC_SUBST([GLIB_LIBS], [`$PKG_CONFIG glib-2.0 --libs`])
             ],
             [AC_CHECK_HEADERS([glib.h], [], 
                        [AC_MSG_ERROR([glib headers not found; set CPPFLAGS to -I glib install directiories or use other unicode handler instead])])])])

### Add here your library ###
#AS_IF([test "x$with_my_transducer_library" != "xno"],
#      [AC_CHECK_HEADERS([my_transducer_library/MyTransducerLibrary.h])])

# Checks for structures
AC_CHECK_DECLS([program_name, program_invocation_name, program_invocation_short_name])
AC_CHECK_DECLS([rl_completion_suppress_append, rl_completion_matches])
AC_CHECK_FUNCS([floor strchr strdup strerror strncasecmp strcspn strtol strtoul error error_at_line strndup getline getdelim getopt_long strtod xstrdup set_program_name setprogname setlocale nl_langinfo])

# Checks for system services

# config files
AC_CONFIG_FILES([Makefile doc/Makefile test/Makefile
                 test/libhfst/Makefile test/tools/Makefile
		 test/tools/fsmbook-examples/Makefile
		 test/tools/fsmbook-examples/EnglishNumerals/Makefile
		 test/tools/fsmbook-examples/NumbersToNumerals/Makefile
		 test/tools/fsmbook-examples/FinnishNumerals/Makefile
		 test/tools/fsmbook-examples/EinsteinsPuzzle/Makefile
		 test/tools/fsmbook-examples/Esperanto/Makefile
		 test/tools/fsmbook-examples/BrazilianPortuguese1/Makefile
		 test/tools/fsmbook-examples/BrazilianPortuguese2/Makefile
                 libhfst/Makefile libhfst/src/Makefile
                 libhfst/src/implementations/Makefile
		 libhfst/src/parsers/Makefile 
                 libhfst/hfst.pc
		 tools/Makefile tools/src/Makefile tools/src/hfst-proc/Makefile
		 tools/src/hfst-twolc/Makefile
		 tools/src/hfst-twolc/src/Makefile
		 tools/src/hfst-twolc/test/Makefile
		 tools/src/hfst-tagger/Makefile
		 tools/src/hfst-tagger/src/Makefile
		 tools/src/hfst-tagger/test/Makefile
		 man/Makefile 
                 scripts/Makefile scripts/hfst-foma-wrapper.sh],
                 [chmod +x scripts/hfst-foma-wrapper.sh])
#		 test/tools/fsmbook-examples/FinnishOTProsody/Makefile
#		 test/tools/fsmbook-examples/YaleShooting/Makefile
#		 test/tools/fsmbook-examples/DateParser/Makefile


# output
AC_OUTPUT

## keep the list alphabetically sorted (vim; !sort)
### Add your library to libhfst targets ###
#    *         with my transducer library: $with_my_transducer_library
cat <<EOF
-- Building $PACKAGE_STRING:
    * libhfst with sfst: $with_sfst
    *         with openfst: $with_openfst
    *         with foma: $with_foma
    *         with optimized lookup: $with_hfstol
    * tools including calculate: $enable_calculate (sfstpl2fst)
    *                 compare: $enable_compare
    *                 compose: $enable_compose
    *                 concatenate: $enable_concatenate
    *                 conjunct: $enable_conjunct (intersect)
    *                 determinize: $enable_determinize (determinise)
    *                 disjunct: $enable_disjunct (union)
    *                 format: $enable_format
    *                 fst2fst: $enable_fst2fst
    *                 fst2strings: $enable_fst2strings (expand)
    *                 fst2txt: $enable_fst2txt
    *                 grep: $enable_grep
    *                 head: $enable_head
    *                 invert: $enable_invert
    *                 lexc: $enable_lexc
    *                 lexc2fst: $enable_lexc_compiler
    *                 lookup: $enable_lookup
    *                 minimize: $enable_minimize (minimise)
    *                 name: $enable_name
    *                 optimized-lookup: $enable_optimized_lookup
    *                 proc: $enable_proc
    *                 project: $enable_project
    *                 push-weights: $enable_push_weights
    *                 regexp2fst: $enable_regexp2fst
    *                 pmatch2fst: $enable_pmatch2fst
    *                 remove-epsilons: $enable_remove_epsilons
    *                 repeat: $enable_repeat
    *                 reverse: $enable_reverse
    *                 shuffle: $enable_shuffle
    *                 split: $enable_split
    *                 strings2fst: $enable_strings2fst
    *                 strip-header: $enable_strip_header
    *                 substitute: $enable_substitute
    *                 subtract: $enable_subtract (minus)
    *                 summarize: $enable_summarize (summarise)
    *                 tail: $enable_tail
    *                 test: $enable_test
    *                 twolc: $enable_twolc
    *                 tagger: $enable_tagger
    *                 txt2fst: $enable_txt2fst
    * foma used for xfst: -> $FOMACLI
    * unicode parsed in corpus tools with: $with_unicode_handler
EOF
dnl stick important warnings to bottom
dnl important licensing information
dnl according to GNU GPLv2 strict (not plus) is incompatible with Apache
AS_IF([test "x$with_foma" != "xno" -a "x$with_openfst" != "xno"],
      [AC_MSG_WARN([foma licensing exception is limited to HFST team’s redistribution; if you consider GPLv2 strict and Apache licence conflicting you may not _redistribute_ your binaries])])
dnl hope end users never disable ofst 
AS_IF([test "x$with_openfst" == "xno"],
      [AC_MSG_WARN([Disabling openfst backend will severely cripple libhfst])])
dnl without foma, foma-based scripts cannot work
AS_IF([test "x$FOMACLI" = "xfalse"],
      [AC_MSG_WARN([using hfst-xfst is disabled because foma binary was not found])])
dnl HFST's case mapping in corpus tools is mainly MES-2 set with other characters
dnl added as we go
AS_IF([test "x$with_unicode_handler" = "xhfst"],
      [AC_MSG_WARN([HFST only supports basic unicode handling with limited case mapping tables etc.; for better support consider using glib or ICU --with-unicode-handler])])
dnl warn about missing "important" tools
AS_IF([test "x$enable_lexc" == "xno"],
      [AC_MSG_WARN([hfst-lexc is not enabled; hfst-lexc2fst toy will only offer limited support to compile lexc lexicons; enable with --enable-lexc])])
AS_IF([test "x$enable_twolc" == "xno"],
      [AC_MSG_WARN([hfst-twolc is not enabled; you will not be able to compile xerox twolc grammars; enable using --enable-twolc])])
AS_IF([test "x$enable_tagger" == "xno"],
      [AC_MSG_WARN([hfst-train-tagger and hfst-tag are not enabled; you will not be able to train and use taggers; enable using --enable-tagger])])
AS_IF([test "x$enable_calculate" == "xno"],
      [AC_MSG_WARN([hfst-calculate is not enabled; you will not be able to compile SFST-PL scripts; enable using --enable-calculate])])

