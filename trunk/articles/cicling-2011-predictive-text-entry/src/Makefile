CXXFLAGS=-Wall -Wextra -Wfatal-errors -O3 
LDFLAGS=-lhfst

PROGS=train_hmm_4_tagger train_sequencer train_morpheme_sequence_model \
      t9_suggest t9_suggest_kpc compute_kpc deleted_interpolation \
      train_hmm_4_tagger_tur train_morpheme_sequence_model_tur \
      t9_suggest_kpc_tur t9_suggest_tur compute_kpc_tur \
      count_average_word_length t9_suggest_only_hmm \
      t9_suggest_only_hmm_and_morpheme

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
Sequencer_EXT_SOURCES=LexicalModel.cc $(LexicalModel_EXT_SOURCES)

all:$(PROGS)

install:all
	cp $(PROGS) ../bin
	cp morfessor1.0.perl ../bin
	chmod u+x ../bin/morfessor1.0.perl
	cp rewrite_morfessor_output.pl ../bin
	chmod u+x ../bin/rewrite_morfessor_output.pl
	cp measure_accuracy.pl ../bin
	chmod u+x ../bin/measure_accuracy.pl
	cp print_errors.pl ../bin
	chmod u+x ../bin/print_errors.pl

clean:
	rm -f $(PROGS)

train_hmm_4_tagger:train_hmm_4_tagger.cc HMM4Tagger.cc $(HMMTagger_EXT_SOURCES)
train_hmm_4_tagger_tur:train_hmm_4_tagger_tur.cc HMM4TaggerTur.cc $(HMMTagger_EXT_SOURCES)
t9_suggest: t9_suggest.cc Sequencer.cc HMMTagger.cc $(HMMTagger_EXT_SOURCES)
t9_suggest_kpc:t9_suggest_kpc.cc Sequencer.cc HMMTagger.cc $(HMMTagger_EXT_SOURCES)
t9_suggest_only_hmm:t9_suggest_only_hmm.cc HMMTagger.cc $(HMMTagger_EXT_SOURCES)
t9_suggest_only_hmm_and_morpheme:t9_suggest_only_hmm_and_morpheme.cc HMMTagger.cc Sequencer.cc $(HMMTagger_EXT_SOURCES)
t9_suggest_kpc_tur:t9_suggest_kpc_tur.cc Sequencer.cc HMMTagger.cc $(HMMTagger_EXT_SOURCES)
t9_suggest_tur:t9_suggest_tur.cc Sequencer.cc HMMTagger.cc $(HMMTagger_EXT_SOURCES)
train_sequencer:train_sequencer.cc Sequencer.cc $(Sequencer_EXT_SOURCES)
train_morpheme_sequence_model:train_morpheme_sequence_model.cc \
                              SequenceModel.cc $(SequenceModel_EXT_SOURCES)
train_morpheme_sequence_model_tur:train_morpheme_sequence_model_tur.cc \
                              SequenceModel.cc $(SequenceModel_EXT_SOURCES)
compute_kpc:KeyStrokesPerCharacter.cc
	g++ -o compute_kpc KeyStrokesPerCharacter.cc -lhfst
compute_kpc_tur:KeyStrokesPerCharacterTur.cc
	g++ -o compute_kpc_tur KeyStrokesPerCharacterTur.cc -lhfst
count_average_word_length:count_average_word_length.cc
	g++ -o $@ $^ -lhfst