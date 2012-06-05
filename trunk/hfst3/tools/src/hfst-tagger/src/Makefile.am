AM_CXXFLAGS= -O3 -g -Wall -Wextra -Wcast-qual -Wno-deprecated -Wfatal-errors
bin_PROGRAMS=hfst-build-tagger
bin_SCRIPTS=compute_data_statistics.py
BUILD_MODEL_SRC=$(srcdir)/build_model_src
USE_MODEL_SRC=$(srcdir)/use_model_src
AM_CPPFLAGS=-I${top_srcdir}/libhfst/src
LDADD = $(top_builddir)/libhfst/src/libhfst.la

hfst-build-tagger_SOURCES:hfst-build-tagger.cc \
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
			  $(BUILD_MODEL_SRC)/string_handling.cc

check_PROGRAMS=FstBuilder LexicalModelBuilder LexicalModelBuilder ModelBuilder\
               SequenceModelBuilder TaggerBuilder \
               WeightedStringVectorCollection string_handling
