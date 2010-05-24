SUBDIRS=implementations
AUTOMAKE_OPTIONS=subdir-objects
lib_LTLIBRARIES = libhfst.la

AM_CXXFLAGS=-Wno-deprecated -Wall -Wextra -g # -DDEBUG_MAIN

# HFST bridge specific stuff
HFST_SRCS=HfstApply.cc HfstInputStream.cc HfstTransducer.cc \
			 HfstOutputStream.cc

# libtool takes over
libhfst_la_SOURCES =  $(HFST_SRCS)
libhfst_la_LIBADD = implementations/libhfstimplementations.la

# we only have one library target but would like a particular includedir
# structure for various headers, so we do this
hfstincludedir = $(includedir)/hfst

HFST_HDRS = \
	HfstTransducer.h \
	apply_schemas.h

hfstinclude_HEADERS = $(HFST_HDRS)

libhfst_la_LDFLAGS = -no-undefined -version-info 1:0:0
