AC_INIT([hfst-proc],[0.1],[brian.s.croom@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile man/Makefile])

#CXXFLAGS="-Wall -Werror -Wextra -march=native -O3"
CXXFLAGS="-Wall -Werror -Wextra -g"
#CXXFLAGS="-Wall -Wextra -Weffc++ -g"

AC_DEFINE([DEBUG], [1], [Print some information useful for debugging])
AC_DEFINE([DEBUG_DIACRITICS], [1], [Don't suppress printing the representations of flag diacritics])
AC_DEFINE([FULL_DEBUG], [0], [Print tons of information about the state of the program])

AC_OUTPUT
