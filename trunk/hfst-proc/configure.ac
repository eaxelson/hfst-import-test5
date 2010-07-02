AC_INIT([hfst-proc],[0.1],[brian.s.croom@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile man/Makefile])

CXXFLAGS="-Wall -Werror -Wextra"

AC_MSG_CHECKING(--enable-debug argument)
AC_ARG_ENABLE(debug,
    [  --enable-debug         Print some information useful for debugging.],
    [enable_debug=$enableval],
    [enable_debug="yes"])
AC_MSG_RESULT($enable_debug)
if test "$enable_debug" = "yes"; then
  AC_DEFINE([DEBUG], [], [Enable debug])
  CXXFLAGS="$CXXFLAGS -g"
else
  CXXFLAGS="$CXXFLAGS -O3"
fi

AC_MSG_CHECKING(--enable-profiling argument)
AC_ARG_ENABLE(profiling,
    [  --enable-profiling         Build with gprof profiling support.],
    [enable_profiling=$enableval],
    [enable_profiling="no"])
AC_MSG_RESULT($enable_profiling)
if test "$enable_profiling" = "yes"; then
  CXXFLAGS="$CXXFLAGS -pg"
fi

AC_DEFINE([DEBUG_DIACRITICS], [1], [Don't suppress printing the representations of flag diacritics])
AC_DEFINE([FULL_DEBUG], [0], [Print tons of information about the state of the program])

AC_OUTPUT
