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
AC_INIT([libhfst], [3.0], [hfst-bugs@helsinki.fi])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# library variables
LIBHFST_NAME=hfst
LIBHFST_MAJOR=3
LIBHFST_MINOR=0
LIBHFST_EXTENSION=_alpha
LIBHFST_VERSION=$LIBHFST_MAJOR.$LIBHFST_MINOR$LIBHFST_EXTENSION
AC_SUBST(LIBHFST_MAJOR)
AC_SUBST(LIBHFST_MINOR)
AC_SUBST(LIBHFST_VERSION)
AC_SUBST(LIBHFST_NAME)

AC_CONFIG_FILES([Makefile src/Makefile src/implementations/Makefile hfst.pc])

AC_CONFIG_SRCDIR([src/HfstTransducer.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# checks for programs
AC_PROG_CXX
ifdef([LT_INIT], [LT_INIT], [AC_PROG_LIBTOOL])
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LEX
AC_PROG_YACC

AC_PATH_PROG([DOXYGEN], [doxygen], [no])
AM_CONDITIONAL([CAN_DOXYGEN], [test x$DOXYGEN != xno])

# if flex 3.X ever comes out, this breaks :)
LEX_VERSION=`$LEX --version`
if test -z "`echo "$LEX_VERSION" | grep 'flex 2\.[[5-9]]\.[[3-9][4-9]]'`"
then
	AC_MSG_WARN([
!! Building hfst successfully requires flex newer than
!! 2.5.33 on some platforms. Unless you're building with
!! pre-flex-generated sources, building will probably fail.])
fi
# '

AC_PATH_PROG([LDCONFIG],[ldconfig],[echo])

# Checks for libraries
AC_CHECK_LIB([pthread], [main])
AC_CHECK_LIB([m], [main])
AC_CHECK_LIB([dl], [main])
AC_CHECK_LIB([z], [main])
# openfst linkage
AC_CHECK_LIB([fst], [main])
AC_CHECK_LIB([fstmain], [main])
# foma
#AC_CHECK_LIB([foma], [main])


# Checks for header files.
AC_CHECK_HEADERS([limits.h stdlib.h string.h])
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([backward/hash_set ext/hash_set hash_set])
AC_CHECK_HEADERS([backward/hash_map ext/hash_map hash_map])
AC_LANG_POP

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor strchr strcspn strdup strtol])

AC_OUTPUT

