AC_PREREQ(2.60)
AC_INIT([hfst-twolc],[1.0],[hfst-bugs@helsinki.fi])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_REVISION([$Revision 1.00$])

PACKAGE=hfst-twolc
MAJOR_VERSION=2
MINOR_VERSION=0
LEX=flex
YACC=bison

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL

AC_LANG([C++])
AC_CHECK_LIB([pthread],[main], , [AC_MSG_ERROR([openfst requires pthread])])
# Disable check for nested build
# AC_CHECK_LIB([hfst],[main], , [AC_MSG_ERROR([libhfst is required])])


# headers needed
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([vector])
AC_CHECK_HEADERS([map])
AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([fstream])
AC_CHECK_HEADERS([cstring])
AC_CHECK_HEADERS([cstdlib])
AC_CHECK_HEADERS([cassert])
AC_LANG_POP([C++])
AC_CHECK_HEADERS([inttypes.h])
AC_CHECK_HEADERS([libintl.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([malloc.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([unistd.h])

# compiler characteristics
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_CONFIG_SRCDIR([src/htwolcpre.yy])

AC_OUTPUT

