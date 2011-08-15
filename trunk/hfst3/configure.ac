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
HFST_MINOR=2
HFST_EXTENSION=0
HFST_VERSION=$HFST_MAJOR.$HFST_MINOR.$HFST_EXTENSION
# for package-config pc file
LIBHFST_NAME=hfst
LIBHFST_MAJOR=3
LIBHFST_MINOR=2
LIBHFST_EXTENSION=0
LIBHFST_VERSION=$LIBHFST_MAJOR.$LIBHFST_MINOR.$LIBHFST_EXTENSION

# autoconf >= 2.62 required for ASSERT, REALLOC
AC_PREREQ([2.62])

# init must be at the start
AC_INIT([hfst], [3.2.0], [hfst-bugs@helsinki.fi], [hfst], [http://hfst.sf.net])
AC_CONFIG_AUX_DIR([build-aux])

# automake >= 1.11 required for color-tests, silent-rules
AM_INIT_AUTOMAKE([1.11 -Wall -Werror std-options foreign check-news color-tests silent-rules])
AM_SILENT_RULES([yes])

# libtool 2 required since some distros totally hose their libtoolses
LT_PREREQ([2.2.6])

AC_REVISION([$Revision: 1.9 $])
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
                            [process unweighted fsts with SFST @<:@default=yes@:>@])],
            [],
            [with_sfst=yes])
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
                            [process unweighted fsts with foma @<:@default=yes@:>@])],
            [],
            [with_foma=yes])
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
                            [select library to use for unicode handling @<:@default=glib@:>@])],
            [with_unicode_handler=$withval],
            [with_unicode_handler=glib])
# make readline optional since mac os x has sucky implementation of readline
AC_ARG_WITH([readline],
            [AS_HELP_STRING([--with-readline],
                            [use readline for interactive programs @<:@default=yes@:>@])],
            [with_readline=$withval],
            [with_readline=yes])

# allow programs to be built separately (sort alphabetically)
dnlAC_ARG_ENABLE([affix_guessify],
dnl			  [AS_HELP_STRING([--enable-affix_guessify],
dnl							  [build affix_guessify tool @<:@default=yes@:>@])],
dnl			  [enable_affix_guessify=$enableval],
dnl			  [enable_affix_guessify=no])
dnl AM_CONDITIONAL([WANT_AFFIX_GUESSIFY], [test x$enable_affix_guessify != xno])
AC_ARG_ENABLE([calculate],
			  [AS_HELP_STRING([--enable-calculate],
							  [build calculate tool @<:@default=yes@:>@])],
			  [enable_calculate=$enableval],
			  [enable_calculate=yes])
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
AC_ARG_ENABLE([flag_diacritics],
			  [AS_HELP_STRING([--enable-flag-diacritics],
							  [build flag diacritic tool @<:@default=no@:>@])],
			  [enable_flag_diacritics=$enableval],
			  [enable_flag_diacritics=no])
AM_CONDITIONAL([WANT_FLAG_DIACRITICS], [test x$enable_flag_diacritics != xno])
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
                              [build lexc cli and readline @<:@default=yes@:>@])],
              [enable_lexc=$enableval],
              [enable_lexc=yes])
AM_CONDITIONAL([WANT_LEXC], [test "x$enable_lexc" != xno])
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
AC_ARG_ENABLE([split],
			  [AS_HELP_STRING([--enable-split],
							  [build split tool @<:@default=yes@:>@])],
			  [enable_split=$enableval],
			  [enable_split=yes])
AM_CONDITIONAL([WANT_SPLIT], [test x$enable_split != xno])
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
							  [build traversal tool @<:@default=yes@:>@])],
			  [enable_traverse=$enableval],
			  [enable_traverse=yes])
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
AC_ARG_ENABLE([xfst-compiler],
			  [AS_HELP_STRING([--enable-xfst-compiler],
							  [build xfst2fst cli tool @<:@default=no@:>@])],
			  [enable_xfst_compiler=$enableval],
			  [enable_xfst_compiler=no])
AM_CONDITIONAL([WANT_XFST_COMPILER], [test x$enable_xfst_compiler != xno])

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
AC_PATH_PROG([GETOPT], [getopt getopts], [false])
AC_CACHE_CHECK([for gnu compatible getopt with long options],
               [ac_cv_path_GNU_GETOPT],
               [AC_PATH_PROGS_FEATURE_CHECK([GNU_GETOPT], 
                                            [getopt ggetopt], 
                            [[m4out=`$ac_path_GNU_GETOPT -V | fgrep getopt`
                             test -z "$m4out" || ac_cv_path_GNU_GETOPT=$ac_path_GNU_GETOPT ac_path_GNU_GETOPT_found=:]],
                            [ac_cv_path_GNU_GETOPT=no])])
AC_SUBST([GNU_GETOPT], [$ac_cv_path_GNU_GETOPT])
AM_CONDITIONAL([CAN_FOMAWRAP], [test x$FOMACLI != xfalse])
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
AC_CHECK_LIB([pthread], [main])
AC_CHECK_LIB([dl], [main])

AC_LANG_PUSH([C++])
AS_IF([test "x$with_openfst" != "xno"], 
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
      [PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.12])
       AC_DEFINE([USE_GLIB_UNICODE], 1, [if using glib for unicode string handling])])
AS_IF([test "x$with_unicode_handler" = "xicu"],
      [PKG_CHECK_MODULES([ICU], [icu-uc >= 4.6])
       AC_DEFINE([USE_ICU_UNICODE], 1, [if using ICU for unicode string handling])])
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

### Add here your library ###
#AS_IF([test "x$with_my_transducer_library" != "xno"],
#      [AC_CHECK_HEADERS([my_transducer_library/MyTransducerLibrary.h])])

AC_HEADER_STDBOOL
AC_HEADER_ASSERT

# Checks for types
AC_TYPE_SIZE_T

# Checks for structures
AC_CHECK_DECLS([program_name, program_invocation_name, program_invocation_short_name])
AC_CHECK_DECLS([rl_completion_suppress_append rl_completion_matches])

# Checks for compiler characteristics
AC_C_INLINE

# Checks for library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor strchr strdup strerror strncasecmp strcspn strtol strtoul error error_at_line strndup getline getdelim getopt_long strtod xstrdup set_program_name setprogname setlocale nl_langinfo])

# Checks for system services

# config files
AC_CONFIG_FILES([Makefile doc/Makefile test/Makefile
                 test/libhfst/Makefile test/tools/Makefile
                 libhfst/Makefile libhfst/src/Makefile
                 libhfst/src/implementations/Makefile
		 libhfst/src/parsers/Makefile 
                 libhfst/hfst.pc
		 tools/Makefile tools/src/Makefile tools/src/hfst-proc/Makefile
		 tools/src/hfst-twolc/Makefile
		 tools/src/hfst-twolc/src/Makefile
		 tools/src/hfst-twolc/test/Makefile
		 man/Makefile 
                 scripts/Makefile scripts/hfst-foma-wrapper.sh],
                 [chmod +x scripts/hfst-foma-wrapper.sh])

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
    * tools including calculate: $enable_calculate (sfst-compiler)
    *                 compare: $enable_compare
    *                 compose: $enable_compose
    *                 concatenate: $enable_concatenate
    *                 conjunct: $enable_conjunct (intersect)
    *                 determinize: $enable_determinize (determinise)
    *                 disjunct: $enable_disjunct (union)
    *                 flag-diacritics: $enable_flag_diacritics
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
    *                 proc: $enable_proc
    *                 project: $enable_project
    *                 push-weights: $enable_push_weights
    *                 regexp2fst: $enable_regexp2fst
    *                 remove-epsilons: $enable_remove_epsilons
    *                 repeat: $enable_repeat
    *                 reverse: $enable_reverse
    *                 split: $enable_split
    *                 strings2fst: $enable_strings2fst
    *                 strip-header: $enable_strip_header
    *                 substitute: $enable_substitute
    *                 subtract: $enable_subtract (minus)
    *                 summarize: $enable_summarize (summarise)
    *                 tail: $enable_tail
    *                 test: $enable_test
    *                 txt2fst: $enable_txt2fst
    *                 xfst2fst: $enable_xfst_compiler
    * foma used for xfst: -> $FOMACLI
    * unicode parsed in corpus tools with: $with_unicode_handler
EOF
dnl stick important warnings to bottom
dnl important licensing information
dnl according to GNU GPLv2 strict (not plus) is incompatible with Apache
AS_IF([test "x$with_foma" != "xno" -a "x$with_openfst" != "xno"],
      [AC_MSG_WARN([NB: foma licensing exception is limited to HFST team’s redistribution; if you consider GPLv2 strict and Apache licence conflicting you may not _redistribute_ your binaries])])
dnl hope end users never disable ofst 
AS_IF([test "x$with_openfst" == "xno"],
      [AC_MSG_WARN([Disabling openfst backend will severely cripple libhfst])])
dnl without gnu getopt, shell scripts cannot support long options or option 
dnl permutation
AS_IF([test "x$GNU_GETOPT" = xno], 
      [AC_MSG_WARN([hfst-xfst will only support part of short options or option permutation, since GNU getopt is missing])])
dnl without foma, foma-based scripts cannot work
AS_IF([test "x$FOMACLI" = "xfalse"],
      [AC_MSG_WARN([using hfst-xfst is disabled because foma binary was not found])])
dnl HFST's case mapping in corpus tools is mainly MES-2 set with other characters
dnl added as we go
AS_IF([test "x$with_unicode_handler" = "xhfst"],
      [AC_MSG_WARN([HFST only supports basic unicode handling with limited case mapping tables etc.; for better support consider using glib or ICU --with-unicode-handler])])

