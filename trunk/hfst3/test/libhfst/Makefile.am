## Process this file with automake to produce Makefile.in
LDADD = ../../libhfst/src/libhfst.la
AM_CPPFLAGS = -I../../libhfst/src/ -I. -g
AM_CXXFLAGS = -Wno-deprecated

# programs to build before unit etc. testing
check_PROGRAMS=test test_constructors test_streams test_tokenizer

# sources for programs
test_SOURCES=test.cc 
test_constructors_SOURCES=test_constructors.cc
test_streams_SOURCES=test_streams.cc
test_tokenizer_SOURCES=test_tokenizer.cc

# programs to run for unit etc. testing
TESTS=test test_constructors test_streams test_tokenizer

# files needed for test programs
# EXTRA_DIST=test.att
