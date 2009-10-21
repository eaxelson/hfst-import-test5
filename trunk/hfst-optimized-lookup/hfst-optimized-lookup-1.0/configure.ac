AC_INIT([hfst-optimized-lookup],[1.0],[hfst-bugs@helsinki.fi])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])

CXXFLAGS="-Wall -Werror -Wextra -march=native -O3"

AC_DEFINE([DEBUG], [0], [Print some information useful for debugging])
AC_DEFINE([TIMING], [0], [Calculate and print timing information (unimplemented currently)])
AC_DEFINE([DEBUG_DIACRITICS], [0], [Don't suppress printing the representations of flag diacritics])
AC_DEFINE([FULL_DEBUG], [0], [Print tons of information about the state of the program])

AC_OUTPUT
