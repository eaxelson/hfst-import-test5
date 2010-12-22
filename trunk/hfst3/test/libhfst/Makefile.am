## Process this file with automake to produce Makefile.in
LDADD = ../../libhfst/src/libhfst.la
AM_CPPFLAGS = -I../../libhfst/src/ -pg
AM_CXXFLAGS = -Wno-deprecated

# programs to build before unit etc. testing
check_PROGRAMS=test

# sources for programs
test_SOURCES=test.cc

# programs to run for unit etc. testing
TESTS=test

# files needed for test programs
# EXTRA_DIST=test.att
