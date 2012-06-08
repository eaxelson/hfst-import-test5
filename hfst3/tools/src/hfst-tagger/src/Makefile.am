AM_CXXFLAGS= -O3 -g -Wall -Wextra -Wcast-qual -Wno-deprecated -Wfatal-errors \
             -I$(top_builddir)/tools/src/inc -I$(top_builddir)/tools/src
bin_PROGRAMS=hfst-build-tagger hfst-open-input-file-for-tagger
bin_SCRIPTS=hfst_tagger_compute_data_statistics.py hfst-train-tagger \
hfst-train-tagger-loc
EXTRA_DIST=hfst-train-tagger.sh
BUILD_MODEL_SRC=$(srcdir)/build_model_src
USE_MODEL_SRC=$(srcdir)/use_model_src
AM_CPPFLAGS=-I${top_srcdir}/libhfst/src
LDADD = $(top_builddir)/libhfst/src/libhfst.la

python_PYTHON=tagger_aux.py hfst_tagger_compute_data_statistics.py

HFST_COMMON_SRC=$(top_srcdir)/tools/src/hfst-program-options.cc \
                $(top_srcdir)/tools/src/hfst-commandline.cc \
	        $(top_srcdir)/tools/src/hfst-tool-metadata.cc

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT=hfst_tagger_compute_data_statistics.py \
                                   hfst-build-tagger \
                                   hfst-open-input-file-for-tagger \
                                   hfst-train-tagger hfst-train-tagger-loc

hfst_build_tagger_SOURCES=hfst-build-tagger.cc \
			  $(BUILD_MODEL_SRC)/FstBuilder.h \
			  $(BUILD_MODEL_SRC)/FstBuilder.cc \
			  $(BUILD_MODEL_SRC)/LexicalModelBuilder.h \
			  $(BUILD_MODEL_SRC)/LexicalModelBuilder.cc \
			  $(BUILD_MODEL_SRC)/ModelBuilder.h \
			  $(BUILD_MODEL_SRC)/ModelBuilder.cc \
			  $(BUILD_MODEL_SRC)/SequenceModelBuilder.h \
			  $(BUILD_MODEL_SRC)/SequenceModelBuilder.cc \
			  $(BUILD_MODEL_SRC)/TaggerBuilder.h \
			  $(BUILD_MODEL_SRC)/TaggerBuilder.cc \
			  $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.h \
			  $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.cc\
			  $(BUILD_MODEL_SRC)/string_handling.h \
			  $(BUILD_MODEL_SRC)/string_handling.cc \
			  $(HFST_COMMON_SRC)


hfst-train-tagger$(EXEEXT): hfst-train-tagger.sh
	cat $< | sed -e 's|BINDIR|$(bindir)|' > $@
	chmod ogu+rx $@

hfst-train-tagger-loc$(EXEEXT): hfst-train-tagger.sh
	cat $< | sed -e 's|BINDIR|$(PWD)|' > $@
	chmod ogu+rx $@

clean-local:
	-rm -f hfst-train-tagger$(EXEEXT) hfst-train-tagger-loc$(EXEEXT)

hfst_open_input_file_for_tagger_SOURCES=hfst-open-input-file-for-tagger.cc \
			  $(HFST_COMMON_SRC)

check_PROGRAMS=FstBuilder LexicalModelBuilder ModelBuilder \
               SequenceModelBuilder TaggerBuilder \
               WeightedStringVectorCollection \
               string_handling

FstBuilder_SOURCES=$(BUILD_MODEL_SRC)/FstBuilder.cc
FstBuilder_LDADD=string_handling.o FstBuilder.o \
                 $(top_srcdir)/libhfst/src/libhfst.la
FstBuilder_CXXFLAGS=-DMAIN_TEST

LexicalModelBuilder_SOURCES=$(BUILD_MODEL_SRC)/LexicalModelBuilder.cc
LexicalModelBuilder_LDADD=string_handling.o FstBuilder.o ModelBuilder.o \
			  WeightedStringVectorCollection.o \
	                  LexicalModelBuilder.o \
                          $(top_srcdir)/libhfst/src/libhfst.la
LexicalModelBuilder_CXXFLAGS=-DMAIN_TEST

SequenceModelBuilder_SOURCES=$(BUILD_MODEL_SRC)/SequenceModelBuilder.cc
SequenceModelBuilder_LDADD=string_handling.o FstBuilder.o ModelBuilder.o \
			  WeightedStringVectorCollection.o \
	                  SequenceModelBuilder.o \
                          $(top_srcdir)/libhfst/src/libhfst.la
SequenceModelBuilder_CXXFLAGS=-DMAIN_TEST

ModelBuilder_SOURCES=$(BUILD_MODEL_SRC)/ModelBuilder.cc
ModelBuilder_LDADD=string_handling.o FstBuilder.o ModelBuilder.o \
		   WeightedStringVectorCollection.o \
                   $(top_srcdir)/libhfst/src/libhfst.la
ModelBuilder_CXXFLAGS=-DMAIN_TEST

WeightedStringVectorCollection_SOURCES=\
	$(BUILD_MODEL_SRC)/WeightedStringVectorCollection.cc
WeightedStringVectorCollection_LDADD=string_handling.o \
	WeightedStringVectorCollection.o \
	$(top_srcdir)/libhfst/src/libhfst.la
WeightedStringVectorCollection_CXXFLAGS=-DMAIN_TEST

TaggerBuilder_SOURCES=$(BUILD_MODEL_SRC)/TaggerBuilder.cc
TaggerBuilder_LDADD=string_handling.o TaggerBuilder.o \
	LexicalModelBuilder.o SequenceModelBuilder.o \
	WeightedStringVectorCollection.o ModelBuilder.o FstBuilder.o \
	$(top_srcdir)/libhfst/src/libhfst.la
TaggerBuilder_CXXFLAGS=-DMAIN_TEST

string_handling_SOURCES=$(BUILD_MODEL_SRC)/string_handling.cc
string_handling_LDADD=string_handling.o $(top_srcdir)/libhfst/src/libhfst.la \
		      unit_test_aux.o
string_handling_CXXFLAGS=-DMAIN_TEST

TESTS=$(check_PROGRAMS)