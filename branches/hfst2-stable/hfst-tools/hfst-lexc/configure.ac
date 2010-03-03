#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([hfst-lexc],[0.6-beta],[tommi.pirinen@helsinki.fi])
AC_CONFIG_AUX_DIR([config-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall foreign dist-bzip2 std-options])
AC_REVISION([$Revision: 1.9 $])

PACKAGE=hfst-lexc
MAJOR_VERSION=0
MINOR_VERSION=5
VERSION_EXT=-beta
VERSION=$MAJOR_VERSION.$MINOR_VERSION$VERSION_EXT

# Options
AC_ARG_ENABLE([debug],
			  [AS_HELP_STRING([--enable-debug],
							  [enable optional debug mode @<:@default=yes@:>@])],
			  [enable_debug=$enableval],
			  [enable_debug=yes])
AC_ARG_ENABLE([timing],
			  [AS_HELP_STRING([--enable-timing],
							  [enable optional timing mode @<:@default=yes@:>@])],
			  [enable_timing=$enableval],
			  [enable_timing=yes])
AC_ARG_ENABLE([yydebug],
			  [AS_HELP_STRING([--enable-yydebug],
			  				  [enable YYDEBUG stuff @<:@default=no@:>@])],
			  [enable_yydebug=$enableval],
			  [enable_yydebug=no])
AC_ARG_ENABLE([static],
			  [AS_HELP_STRING([--enable-static],
			  				  [enable static linking for legacy systems @<:@default=no@:>@])],
			  [enable_static=$enableval],
			  [enable_static=no])
if test x$enable_debug != xno ; then
	AC_DEFINE([DEBUG], 1, [Define to enable debugging printouts and saves])
fi
if test x$enable_timing != xno ; then
	AC_DEFINE([TIMING], 1, [Define to enable timing calls and printouts])
fi
if test x$enable_yaccdebug != xno ; then
	AC_DEFINE([YYDEBUG], 1, [Define to enable standard yacc debug prints])
fi
AC_DEFINE([SGIext], 1, [SFST requires this stuff])

# Checks for programs
ifdef([LT_INIT], [LT_INIT], [AC_PROG_LIBTOOL])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LEX
AC_PROG_YACC

# testing
AC_PATH_PROG([VALGRIND], [valgrind], [no])
AM_CONDITIONAL([CAN_VALGRIND], [test x$VALGRIND != xno])
AC_PATH_PROG([HFSTPAIRTEST], [hfst-pair-test], [no])
AM_CONDITIONAL([CAN_PAIRTEST], [test x$HFSTPAIRTEST != xno])

# Checks for libraries
AC_LANG([C++])
AC_CHECK_LIB([pthread], [main], , [AC_MSG_ERROR([openfst requires pthread])])

# Checks for header files
AC_HEADER_STDC
AC_HEADER_STDBOOL

# most of these are needed by flex/bison
AC_CHECK_HEADERS([inttypes.h])
AC_CHECK_HEADERS([libintl.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([malloc.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([sys/stat.h])

AC_HEADER_ASSERT

AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC



# Checks for typedefs, structures and compiler characteristics
AC_C_CONST

AC_TYPE_SIZE_T

AC_CHECK_TYPES([ptrdiff_t])


# Output files
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile test/omorfi/Makefile test/divvun-sme/Makefile test/morphalou/Makefile test/kos83/Makefile man/Makefile test/xre.at-file.hlexc test/fao/Makefile])
AC_CONFIG_HEADERS([config.h])
AC_DEFINE([NESTED_BUILD], [1], [We're doing a nested build, headers aren't always where we expect them to be])
AC_CONFIG_SRCDIR([src/lexc.h])
AC_OUTPUT

# be informative yay
cat <<EOF
-- Building $PACKAGE v$VERSION:
	* debugging mode: $enable_debug
	* YYDEBUG: $enable_yydebug
	* timing mode: $enable_timing
	* valgrind: $VALGRIND
	* doxygen: $DOXYGEN
	* rst: $RST2HTML
	* pair-test: $HFSTPAIRTEST
EOF
