## Process this file with autoconf to produce configure script
AC_INIT([hfst], [3.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SUBDIRS([libhfst tools])

# Versioning of hfst-package; relevant for package naming
HFST_NAME=hfst
HFST_MAJOR=3
HFST_MINOR=0
HFST_EXTENSION=_alpha
HFST_VERSION=$HFST_MAJOR.$HFST_MINOR$HFST_EXTENSION

# for tests written in C++ add stuff here
AC_PROG_CXX
ifdef([LT_INIT], [LT_INIT], [AC_PROG_LIBTOOL])
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP

# libhfst's SFST part
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([backward/hash_set ext/hash_set hash_set])
AC_CHECK_HEADERS([backward/hash_map ext/hash_map hash_map])
AC_LANG_POP


AC_OUTPUT([Makefile doc/Makefile test/Makefile test/libhfst/Makefile])

