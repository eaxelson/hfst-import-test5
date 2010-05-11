AM_CPPFLAGS =-pg -I$(srcdir)/../include
AM_CFLAGS=$(AM_CFLAGS) -O2 -pg 
lib_LTLIBRARIES = libfst.la
libfst_la_SOURCES = compat.cc flags.cc fst.cc properties.cc \
symbol-table.cc util.cc
libfst_la_LDFLAGS = -version-info 0:0:0 -pg

