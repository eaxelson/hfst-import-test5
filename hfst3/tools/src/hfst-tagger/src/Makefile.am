AM_CXXFLAGS= -O3 -g -Wall -Wextra -Wcast-qual -Wno-deprecated -Wfatal-errors \
             -I$(top_builddir)/tools/src/inc -I$(top_builddir)/tools/src
bin_PROGRAMS=hfst-build-tagger hfst-open-input-file-for-tagger
bin_SCRIPTS=compute_data_statistics.py hfst-train-tagger hfst-train-tagger-loc
EXTRA_DIST=hfst-train-tagger.sh
BUILD_MODEL_SRC=$(srcdir)/build_model_src
USE_MODEL_SRC=$(srcdir)/use_model_src
AM_CPPFLAGS=-I${top_srcdir}/libhfst/src
LDADD = $(top_builddir)/libhfst/src/libhfst.la

HFST_COMMON_SRC=$(top_srcdir)/tools/src/hfst-program-options.cc \
                $(top_srcdir)/tools/src/hfst-commandline.cc \
	        $(top_srcdir)/tools/src/hfst-tool-metadata.cc

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT=compute_data_statistics.py \
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

check_PROGRAMS=FstBuilder LexicalModelBuilder ModelBuilder\
               SequenceModelBuilder TaggerBuilder \
               WeightedStringVectorCollection string_handling

FstBuilder_SOURCES=$(BUILD_MODEL_SRC)/FstBuilder.h \
		   $(BUILD_MODEL_SRC)/FstBuilder.cc \
		   $(BUILD_MODEL_SRC)/string_handling.h \
		   $(BUILD_MODEL_SRC)/string_handling.cc

LexicalModelBuilder_SOURCES=$(BUILD_MODEL_SRC)/LexicalModelBuilder.h \
			    $(BUILD_MODEL_SRC)/LexicalModelBuilder.cc \
			    $(BUILD_MODEL_SRC)/FstBuilder.cc \
			    $(BUILD_MODEL_SRC)/FstBuilder.h \
			    $(BUILD_MODEL_SRC)/ModelBuilder.cc \
			    $(BUILD_MODEL_SRC)/ModelBuilder.h \
			    $(BUILD_MODEL_SRC)/string_handling.cc \
			    $(BUILD_MODEL_SRC)/string_handling.h \
			 $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.cc \
			 $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.h

ModelBuilder_SOURCES=$(BUILD_MODEL_SRC)/ModelBuilder.h \
		     $(BUILD_MODEL_SRC)/ModelBuilder.cc \
		     $(BUILD_MODEL_SRC)/FstBuilder.cc \
		     $(BUILD_MODEL_SRC)/FstBuilder.h \
		     $(BUILD_MODEL_SRC)/string_handling.cc \
	             $(BUILD_MODEL_SRC)/string_handling.h \
	             $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.cc \
	             $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.h

SequenceModelBuilder_SOURCES=$(BUILD_MODEL_SRC)/SequenceModelBuilder.h \
			     $(BUILD_MODEL_SRC)/SequenceModelBuilder.cc \
			     $(BUILD_MODEL_SRC)/FstBuilder.cc \
			     $(BUILD_MODEL_SRC)/FstBuilder.h \
			     $(BUILD_MODEL_SRC)/ModelBuilder.cc \
			     $(BUILD_MODEL_SRC)/ModelBuilder.h \
			     $(BUILD_MODEL_SRC)/string_handling.cc \
			     $(BUILD_MODEL_SRC)/string_handling.h \
			 $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.cc \
			 $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.h

TaggerBuilder_SOURCES=$(BUILD_MODEL_SRC)/TaggerBuilder.h \
		      $(BUILD_MODEL_SRC)/TaggerBuilder.cc \
                      $(BUILD_MODEL_SRC)/SequenceModelBuilder.h \
		      $(BUILD_MODEL_SRC)/SequenceModelBuilder.cc \
		      $(BUILD_MODEL_SRC)/FstBuilder.cc \
		      $(BUILD_MODEL_SRC)/FstBuilder.h \
		      $(BUILD_MODEL_SRC)/ModelBuilder.cc \
		      $(BUILD_MODEL_SRC)/ModelBuilder.h \
		      $(BUILD_MODEL_SRC)/string_handling.cc \
		      $(BUILD_MODEL_SRC)/string_handling.h \
	              $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.cc \
		      $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.h

WeightedStringVectorCollection_SOURCES= \
                         $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.cc \
                         $(BUILD_MODEL_SRC)/WeightedStringVectorCollection.h \
                         $(BUILD_MODEL_SRC)/string_handling.h \
                         $(BUILD_MODEL_SRC)/string_handling.cc

string_handling_SOURCES=$(BUILD_MODEL_SRC)/string_handling.h \
			$(BUILD_MODEL_SRC)/string_handling.cc

TESTS=$(check_PROGRAMS)