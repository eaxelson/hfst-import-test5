WARNINGS=-Wall -Wextra -Wfatal-errors

PROGS=train_hmm_tagger tag_with_hmm_tagger context_speller

# The source X.cc for each test program TestX is listed in TEST_SRC
TEST_SRCS=LastEntryRemover.cc StringTransformerList.cc SequenceWeighter.cc \
          WordformRemover.cc SequenceWeightTable.cc StreamMarkovizer.cc \
          LogSequenceWeightTable.cc ModelBuilder.cc CycleBuilder.cc \
	  CompletedCycleBuilder.cc StreamFieldInverter.cc StreamTransformer.cc\
	  StreamReinitializer.cc StreamTokenizer.cc \
	  LexiconDivisorTransformer.cc TrieBuilder.cc IdentityTransformer.cc \
	  GuesserStreamTransformer.cc LexicalModel.cc \
	  GuesserDivisorTransformer.cc LexiconBuilder.cc GuesserBuilder.cc \
	  LexicalStreamTransformer.cc WordformProbabilityListBuilder.cc \
	  ToEmptyStringTransformer.cc WordformProbabilityListTransformer.cc \
          ComposeIntersectNGramModel.cc ComposeIntersectNGramModelPair.cc \
          NGramAbstracter.cc NGramModelConstructor.cc SequenceModel.cc \
          StandardHMMLexicalModel.cc Tagger.cc HMMTagger.cc SentenceStream.cc \
          Speller.cc SuggestionsInContext.cc

TEST_PROGS=$(TEST_SRCS:%.cc=Test%)

# If the test program TestX requires additional source code files besides X.cc,
# they should be listed in X_EXT_SOURCES.
SequenceWeighter_EXT_SOURCES=SequenceCounter.cc
SequenceWeightTable_EXT_SOURCES=StringTransformerList.cc SequenceCounter.cc \
                                SequenceWeighter.cc WordformRemover.cc \
	  			LastEntryRemover.cc 
LogSequenceWeightTable_EXT_SOURCES=SequenceWeightTable.cc \
                                   $(SequenceWeightTable_EXT_SOURCES)
CycleBuilder_EXT_SOURCES=ModelBuilder.cc LogSequenceWeightTable.cc \
                         $(LogSequenceWeightTable_EXT_SOURCES)
CompletedCycleBuilder_EXT_SOURCES=CycleBuilder.cc $(CycleBuilder_EXT_SOURCES)
StreamFieldInverter_EXT_SOURCES=StreamTransformer.cc
StreamMarkovizer_EXT_SOURCES=StreamTransformer.cc
StreamReinitializer_EXT_SOURCES=StreamTransformer.cc
StreamTokenizer_EXT_SOURCES=StreamTransformer.cc
LexicalStreamTransformer_EXT_SOURCES=StreamTokenizer.cc StreamFieldInverter.cc\
				     $(StreamFieldInverter_EXT_SOURCES)
TrieBuilder_EXT_SOURCES=ModelBuilder.cc $(ModelBuilder_EXT_SOURCES) \
                        SequenceWeightTable.cc \
                        $(SequenceWeightTable_EXT_SOURCES) \
			IdentityTransformer.cc LexiconDivisorTransformer.cc \
			StreamTokenizer.cc StreamTransformer.cc \
			LogSequenceWeightTable.cc
GuesserStreamTransformer_EXT_SOURCES=StreamTransformer.cc \
                                     $(StreamTransformer_EXT_SOURCES)
WordformProbabilityListTransformer_EXT_SOURCES=StreamTransformer.cc \
                                               $(StreamTransformer_EXT_SOURCES)
LexiconBuilder_EXT_SOURCES=TrieBuilder.cc $(TrieBuilder_EXT_SOURCES) 
GuesserBuilder_EXT_SOURCES=TrieBuilder.cc $(TrieBuilder_EXT_SOURCES) \
			   GuesserDivisorTransformer.cc \
			   GuesserStreamTransformer.cc 
WordformProbabilityListBuilder_EXT_SOURCES=\
                               TrieBuilder.cc $(TrieBuilder_EXT_SOURCES) \
                               ToEmptyStringTransformer.cc \
                               WordformProbabilityListTransformer.cc
LexicalModel_EXT_SOURCES=LexiconBuilder.cc \
		         GuesserBuilder.cc \
		         $(GuesserBuilder_EXT_SOURCES) \
                         StreamReinitializer.cc \
                         LexicalStreamTransformer.cc \
		         StreamFieldInverter.cc \
			 LanguageModel.cc \
		         WordformProbabilityListBuilder.cc \
                         ToEmptyStringTransformer.cc \
                         WordformProbabilityListTransformer.cc
StandardHMMLexicalModel_EXT_SOURCES=$(LexicalModel_EXT_SOURCES) LexicalModel.cc
ComposeIntersectNGramModelPair_EXT_SOURCES=ComposeIntersectNGramModel.cc
NGramModelConstructor_EXT_SOURCES=NGramAbstracter.cc CompletedCycleBuilder.cc\
				  $(CompletedCycleBuilder_EXT_SOURCES) \
				  StreamMarkovizer.cc \
				  $(StreamMarkovizer_EXT_SOURCES)
SequenceModel_EXT_SOURCES=$(NGramModelConstructor_EXT_SOURCES) \
                          NGramModelConstructor.cc \
	                  ComposeIntersectNGramModelPair.cc \
                          $(ComposeIntersectNGramModelPair_EXT_SOURCES) \
                          StreamReinitializer.cc
Tagger_EXT_SOURCES=SequenceModel.cc $(SequenceModel_EXT_SOURCES) \
                      StandardHMMLexicalModel.cc LexicalModel.cc \
                      LexiconBuilder.cc GuesserBuilder.cc \
                      WordformProbabilityListBuilder.cc LanguageModel.cc \
                      IdentityTransformer.cc LexicalStreamTransformer.cc \
                      GuesserDivisorTransformer.cc \
                      LexiconDivisorTransformer.cc TrieBuilder.cc \
                      ToEmptyStringTransformer.cc GuesserStreamTransformer.cc \
                      StreamFieldInverter.cc \
                      WordformProbabilityListTransformer.cc \
                      StreamTokenizer.cc
HMMTagger_EXT_SOURCES=Tagger.cc $(Tagger_EXT_SOURCES)
Speller_EXT_SOURCES=HMMTagger.cc $(HMMTagger_EXT_SOURCES) \
                    SuggestionsInContext.cc

# If the test program TestX requires external libraries, they should be listed
# in X_LIBS.
ModelBuilder_LIBS=-lhfst
CycleBuilder_LIBS=-lhfst
CompletedCycleBuilder_LIBS=-lhfst
StreamFieldInverter_LIBS=-lhfst
StreamTokenizer_LIBS=-lhfst
LexicalStreamTransformer_LIBS=-lhfst
TrieBuilder_LIBS=-lhfst
LexicalModel_LIBS=-lhfst
StandardHMMLexicalModel_LIBS=-lhfst
LexiconBuilder_LIBS=-lhfst
GuesserBuilder_LIBS=-lhfst
WordformProbabilityListBuilder_LIBS=-lhfst
ComposeIntersectNGramModel_LIBS=-lhfst
ComposeIntersectNGramModelPair_LIBS=-lhfst
NGramModelConstructor_LIBS=-lhfst
SequenceModel_LIBS=-lhfst
Tagger_LIBS=-lhfst
HMMTagger_LIBS=-lhfst
Speller_LIBS=-lhfst

all:$(PROGS)

test:test_progs
	for t in `ls Test*`; do ./"$$t"; done

clean:
	rm -f $(TEST_PROGS) $(PROGS)

test_progs: $(TEST_PROGS)

Test%:%.cc 
	g++ $(WARNINGS) -o $@ $^ $($*_EXT_SOURCES) -DHFST_TAGGER_TEST_$* $($*_LIBS)

train_hmm_tagger:train_hmm_tagger.cc HMMTagger.cc $(HMMTagger_EXT_SOURCES)
	g++ $(WARNINGS) -O3 -o $@ $^ -lhfst

tag_with_hmm_tagger:tag_with_hmm_tagger.cc HMMTagger.cc \
                    $(HMMTagger_EXT_SOURCES) SentenceStream.cc
	g++ $(WARNINGS) -g -O3 -o $@ $^ -lhfst

context_speller:context_speller.cc Speller.cc $(Speller_EXT_SOURCES)
	g++ $(WARNINGS) -g -O3 -o $@ $^ -lhfst
