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



# autoconf >= 2.62 required for ASSERT, REALLOC
AC_PREREQ([2.62])

# init must be at the start
AC_INIT([libhfst], [3.3.12f], [hfst-bugs@helsinki.fi], [libhfst], [http://hfst.sf.net])
AC_CONFIG_AUX_DIR([build-aux])

# automake >= 1.11 required for color-tests, silent-rules
AM_INIT_AUTOMAKE([1.11 -Wall -Werror std-options foreign check-news color-tests silent-rules])
AM_SILENT_RULES([yes])

# libtool 2 required since some distros totally hose their libtoolses
LT_PREREQ([2.2.6])

AC_REVISION([$Revision$])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/HfstTransducer.h])
AC_CONFIG_HEADERS([config.h src/hfst.hpp])

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

# Disable stress tests by default
AC_ARG_ENABLE([stress-tests],
              [AS_HELP_STRING([--enable-stress-tests],
                              [include stress tests in check @<:@default=no@:>@])],
                              [enable_stress_tests=$enableval],
                              [enable_stress_tests=no])
AM_CONDITIONAL([WANT_STRESSTESTS], [test x$enable_stress_tests != xno])

# subst in hfst.pc.in
AC_SUBST([HFST_MAJOR], [3])
AC_SUBST([HFST_MINOR], [3])
AC_SUBST([HFST_EXTENSION], [14f])
AC_SUBST([HFST_VERSION], [3.3.14f])
AC_SUBST([HFST_NAME], [hfst])

# long version = version vector cast in base 10000, for automatic comparisons
# e.g.: 3.3.12 = 0003 0000 0000 + 0003 0000 + 0012
AC_DEFINE([HFST_LONGVERSION], [300030014L],
          [Define to hfst version vector as long in base 10000])
AC_DEFINE([HFST_REVISION], ["$Revision$"],
          [Automatically substitute to configure.ac revision])
AC_DEFINE_UNQUOTED([HFST_STRING], ["$PACKAGE_STRING"],
          [Define to libhfst pretty name for programs to print])

# Checks for programs
AC_PROG_CC
AC_PROG_CXX
LT_INIT
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_YACC
AC_PROG_LEX

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
      [AC_CHECK_LIB([sfst1], [main])]
      [AS_IF([test "x$ac_cv_lib_sfst_main" == xno -a "x$ac_cv_lib_sfst1_main" == xno],
             [AC_MSG_FAILURE([sfst tests failed (--without-sfst to disable)])])])
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

AS_IF([test "x$enable_apertium2fst" != "xno"],
      [PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= 2.6.19])])

### Add here your library ###
#AS_IF([test "x$with_my_transducer_library" != "xno"],
#      [AC_CHECK_LIB([my_transducer_library], [main], [],
#                [AC_MSG_FAILURE([my transducer library test failed (--without-my-transducer-library to disable)])])])

# Checks for header files
AC_CHECK_HEADERS([limits.h stdlib.h string.h])

AC_LANG_PUSH([C++])

AS_IF([test "x$with_sfst" != "xno"],
      [AC_CHECK_HEADERS([sfst/fst.h sfst1/fst.h])]
      [AC_CHECK_HEADERS([backward/hash_set ext/hash_set hash_set])]
      [AC_CHECK_HEADERS([backward/hash_map ext/hash_map hash_map])]
      [AC_DEFINE([_GLIBCXX_PERMIT_BACKWARD_HASH], [1], 
                 [define to silence gcc warnings from SFST using obsolete data structures])])
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

# Checks for compiler characteristics
AC_C_INLINE

# Checks for library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor strchr strdup strncasecmp strcspn strtol strtoul strndup strtod])

# Checks for system services

# config files
AC_CONFIG_FILES([Makefile doc/Makefile test/Makefile
                 src/Makefile
                 src/implementations/Makefile
                 src/parsers/Makefile 
                 hfst.pc])

# output
AC_OUTPUT

## keep the list alphabetically sorted (vim; !sort)
cat <<EOF
-- Building $PACKAGE_STRING:
    * libhfst with sfst: $with_sfst
    *         with openfst: $with_openfst
    *         with foma: $with_foma
    *         with optimized lookup: $with_hfstol
EOF
### Add your library to libhfst targets ###
#    *         with my transducer library: $with_my_transducer_library

# stick important warnings to bottom
# important licensing information
# according to GNU GPLv2 strict (not plus) is incompatible with Apache
AS_IF([test "x$with_foma" != "xno" -a "x$with_openfst" != "xno"],
      [AC_MSG_WARN([NB: foma licensing exception is limited to HFST team’s 
       redistribution; if you consider GPLv2 strict and Apache licence 
       conflicting you may not _redistribute_ your binaries])])
# hope end users never disable ofst 
AS_IF([test "x$with_openfst" == "xno"],
      [AC_MSG_WARN([Disabling openfst backend will severely cripple libhfst])])

AC_MSG_WARN([You are using development branch of broken up packages. No support
             is provided, please revert to trunk unless alpha testing])

