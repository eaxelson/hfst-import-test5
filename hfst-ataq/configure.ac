## Process this file with autoconf to produce configure script

## Copyright (C) 2010 University of Helsinki
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.


# autoconf requirements
AC_PREREQ([2.62])
LT_PREREQ([2.2.6])

# init
AC_INIT([hfst-ataq], [0.0.0], [hfst-bugs@helsinki.fi], [hfst-ataq], [http://hfst.sf.net])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.12 -Wall -Werror foreign check-news color-tests silent-rules])
AM_SILENT_RULES([yes])
AC_REVISION([$Revision: 3876 $])
AC_CONFIG_MACRO_DIR([m4])
#AC_CONFIG_SRCDIR([ataq.cc])
AC_CONFIG_HEADERS([config.h])

# Information on package
HFSTATAQ_NAME=hfstataq
HFSTATAQ_MAJOR=0
HFSTATAQ_MINOR=0
HFSTATAQ_EXTENSION=.0
HFSTATAQ_VERSION=$HFSTATAQ_MAJOR.$HFSTATAQ_MINOR$HFSTATAQ_EXTENSION
AC_SUBST(HFSTATAQ_MAJOR)
AC_SUBST(HFSTATAQ_MINOR)
AC_SUBST(HFSTATAQ_VERSION)
AC_SUBST(HFSTATAQ_NAME)

# Check for pkg-config first - the configuration won't work if it isn't available:
AC_PATH_PROG([PKGCONFIG], [pkg-config], [no])
AS_IF([test "x$PKGCONFIG" = xno], [AC_MSG_ERROR([pkg-config is required - please install])])

# Settings

# Checks for programs
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_CC
AC_PROG_CXX
AC_LIBTOOL_WIN32_DLL
LT_INIT
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries

# Checks for header files
AC_CHECK_HEADERS([getopt.h error.h])

# Checks for types
AC_TYPE_SIZE_T

# Checks for structures

# Checks for compiler characteristics

# Checks for library functions
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strndup error])
# Checks for system services

# config files
AC_CONFIG_FILES([Makefile hfstataq.pc])

# output
AC_OUTPUT

cat <<EOF
-- Building $PACKAGE_STRING
EOF
