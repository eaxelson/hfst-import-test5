## Process this file with automake to produce Makefile.in

# Copyright 2010 University of Helsinki
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

# to silence: 
# libtoolize: Consider adding `-I m4' to ACLOCAL_AMFLAGS in Makefile.am.
ACLOCAL_AMFLAGS=-I m4

# targets
bin_PROGRAMS=hfst-ataq
lib_LTLIBRARIES=libhfstataq.la
man1_MANS=hfst-ataq.1

PKG_LIBS=
PKG_CXXFLAGS=

# library parts
libhfstataq_la_SOURCES=ataq.cc
libhfstataq_la_CXXFLAGS=$(AM_CXXFLAGS) $(CXXFLAGS) $(PKG_CXXFLAGS)
libhfstataq_la_LDFLAGS=-no-undefined -version-info 0:0:0

# link sample program against library here
hfst_ataq_SOURCES=main.cc
hfst_ataq_LDADD=libhfstataq.la
hfst_ataq_CXXFLAGS=$(AM_CXXFLAGS) $(CXXFLAGS) $(PKG_CXXFLAGS)

# install headers for library in hfst's includedir
include_HEADERS=hfst-ataq.h

# pkgconfig
pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=hfstataq.pc

# tests

