AC_INIT([libhfst], [2.4], [hfst-bugs@helsinki.fi])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_FILES([Makefile])

AC_CONFIG_SRCDIR([datatypes.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

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

# if flex 3.X ever comes out, this breaks :)
LEX_VERSION=`$LEX --version`
if test -z "`echo "$LEX_VERSION" | grep 'flex 2\.[[5-9]]\.[[3-9][4-9]]'`"
then
	AC_MSG_WARN([
!! Building hfst successfully requires flex newer than
!! 2.5.33 on some platforms. Unless you're building with
!! pre-flex-generated sources, building will probably fail.])
fi


AC_PATH_PROG([LDCONFIG],[ldconfig],[echo])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor strchr strcspn strdup strtol])

# AC_CHECK_HEADERS([]) # Maybe one day...

AC_OUTPUT