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

# autoconf requirements
AC_PREREQ([2.62])

# init
AC_INIT([hfst], [3.0.1], [hfst-bugs@helsinki.fi], [hfst], [http://hfst.sf.net])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror std-options foreign check-news color-tests silent-rules 1.11])
AM_SILENT_RULES([yes])
AC_REVISION([$Revision: 1.9 $])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([libhfst/src/HfstTransducer.h])
AC_CONFIG_HEADERS([config.h])

# Information on package
HFST_NAME=hfst
HFST_MAJOR=3
HFST_MINOR=0
HFST_EXTENSION=1	
HFST_VERSION=$HFST_MAJOR.$HFST_MINOR.$HFST_EXTENSION
# for package-config pc file
LIBHFST_NAME=hfst
LIBHFST_MAJOR=3
LIBHFST_MINOR=0
LIBHFST_EXTENSION=1
LIBHFST_VERSION=$LIBHFST_MAJOR.$LIBHFST_MINOR.LIBHFST_EXTENSION
AC_SUBST(LIBHFST_MAJOR)
AC_SUBST(LIBHFST_MINOR)
AC_SUBST(LIBHFST_EXTENSION)
AC_SUBST(LIBHFST_VERSION)
AC_SUBST(LIBHFST_NAME)

# allow backends to be removed
AC_ARG_WITH([sfst],
            [AS_HELP_STRING([--with-sfst],
                            [process unweighted fsts with SFST @<:@default=check@:>@])],
            [],
            [with_sfst=check])
AS_IF([test "x$with_sfst" != xno], [AC_DEFINE([HAVE_SFST], [1],
                                              [Define to compile SFST support in HFST])])
AM_CONDITIONAL([WANT_SFST], [test x$with_sfst != xno])
AC_ARG_WITH([openfst],
            [AS_HELP_STRING([--with-openfst],
                            [process weighted fsts with OpenFst @<:@default=check@:>@])],
            [],
            [with_openfst=check])
AS_IF([test "x$with_openfst" != xno], [AC_DEFINE([HAVE_OPENFST], [1],
                                              [Define to compile OpenFst support in HFST])])
AM_CONDITIONAL([WANT_OPENFST], [test x$with_openfst != xno])
AC_ARG_WITH([foma],
            [AS_HELP_STRING([--with-foma],
                            [process unweighted fsts with foma @<:@default=check@:>@])],
            [],
            [with_foma=check])
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


# allow programs to be built separately (sort alphabetically)
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
AC_ARG_ENABLE([lexc-compiler],
              [AS_HELP_STRING([--enable-lexc-compiler],
                              [build lexc2fst cli @<:@default=no@:>@])],
              [enable_lexc_compiler=$enableval],
              [enable_lexc_compiler=no])
AM_CONDITIONAL([WANT_LEXC_COMPILER], [test "x$enable_lexc_compiler" != xno])
AC_ARG_ENABLE([lookup],
			  [AS_HELP_STRING([--enable-lookup],
							  [build lookup tool @<:@default=yes@:>@])],
			  [enable_lookup=$enableval],
			  [enable_lookup=yes])
AM_CONDITIONAL([WANT_LOOKUP], [test x$enable_lookup != xno])
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

# Checks for programs
AC_PROG_CC
AC_PROG_CXX
m4_ifdef([LT_INIT], [LT_INIT], [AC_PROG_LIBTOOL])
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_YACC
AC_PROG_LEX
AC_PATH_PROG([FOMACLI], [foma], [false])
AM_CONDITIONAL([CAN_FOMAWRAP], [test x$FOMACLI != xfalse])
AS_IF([test x$FOMACLI = xfalse], [AC_MSG_WARN([You cannot use hfst-lexc or hfst-xfst without available foma binaries])])
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

AS_IF([test "x$with_openfst" != "xno"], 
      [AC_CHECK_LIB([fst], [main], [],
                 [AC_MSG_FAILURE([openfst test failed (--without-openfst to disable)])])]
      [AC_CHECK_LIB([m], [main])])

AS_IF([test "x$with_sfst" != "xno"],
      [AC_CHECK_LIB([sfst], [main], [],
                 [AC_MSG_FAILURE([sfst test failed (--without-sfst to disable)])])])

AS_IF([test "x$with_foma" == "xcheck"],
      [AC_CHECK_LIB([z], [main])]
      [AC_CHECK_LIB([foma], [main], [],
                 [AC_MSG_FAILURE([foma test failed (--without-foma to disable)])])])

### Add here your library ###
#AS_IF([test "x$with_my_transducer_library" != "xno"],
#      [AC_CHECK_LIB([my_transducer_library], [main], [],
#                [AC_MSG_FAILURE([my transducer library test failed (--without-my-transducer-library to disable)])])])

# Checks for header files
AC_CHECK_HEADERS([limits.h stdlib.h string.h error.h glob.h])

AC_LANG([C++])

AS_IF([test "x$with_sfst" != "xno"],
      [AC_CHECK_HEADERS([sfst/fst.h])]
      [AC_CHECK_HEADERS([backward/hash_set ext/hash_set hash_set])]
      [AC_CHECK_HEADERS([backward/hash_map ext/hash_map hash_map])])
AS_IF([test "x$with_openfst" != "xno"],
      [AC_CHECK_HEADERS([fst/fstlib.h])])
AS_IF([test "x$with_foma" != "xno"],
      [AC_LANG_PUSH([C])]
      [AC_CHECK_HEADERS([fomalib.h])]
      [AC_LANG_POP])
### Add here your library ###
#AS_IF([test "x$with_my_transducer_library" != "xno"],
#      [AC_CHECK_HEADERS([my_transducer_library/MyTransducerLibrary.h])])

AC_HEADER_STDBOOL
AC_HEADER_ASSERT

# Checks for types
AC_TYPE_SIZE_T

# Checks for structures
AC_CHECK_DECLS([program_name, program_invocation_name, program_invocation_short_name])

# Checks for compiler characteristics
AC_C_INLINE

# Checks for library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor strchr strdup strerror strncasecmp strcspn strtol strtoul error error_at_line strndup getline getopt_long strtod xstrdup set_program_name setprogname])

# Checks for system services

# config files
AC_CONFIG_FILES([Makefile doc/Makefile test/Makefile
                 test/libhfst/Makefile test/tools/Makefile
                 libhfst/Makefile libhfst/src/Makefile
                 libhfst/src/implementations/Makefile 
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
    *                 head: $enable_head
    *                 invert: $enable_invert
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
    *                 foma=xfst/lexc: -> $FOMACLI
EOF
dnl important licencing information
dnl according to GNU GPLv2 strict (not plus) is incompatible with Apache
AS_IF([test "x$with_foma" != "xno" -a "x$with_openfst" != "xno"],
      [AC_MSG_WARN([Backends with conflicting licences selected (foma: GPLv2 conflicts OpenFst: Apache), resulting binary must not be redistributed])])
dnl hope end users never disable ofst 
AS_IF([test "x$with_openfst" == "xno"],
      [AC_MSG_WARN([Disabling openfst backend will severely cripple libhfst])])

