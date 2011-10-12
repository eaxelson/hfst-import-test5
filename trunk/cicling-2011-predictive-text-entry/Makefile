BIN=bin
FSTS=fsts
SRC=src
FST_SRC=fst_src
FST_BIN=fsts
FI_DATA=data/finnish
FI_PROCESSED_DATA=processed_training_data/finnish
FI_RESULTS=results/finnish
MODELS=models
FSTS=$(FST_BIN)/text_to_numbers.hfst $(FST_BIN)/combinatorics.hfst
TRAINING_DATA_SIZES=1000 35000 69000 103000 137000 171000 205000 239000 \
                    307000 341000 345398

all:tools fsts training_data all-results

clean:
	-rm -f $(BIN)/*
	-rm -f $(FST_BIN)/*
	-rm -f $(FI_PROCESSED_DATA)/*
	-rm -f $(MODELS)/*
	-rm -f $(FI_RESULTS)/*
	$(MAKE) -C $(SRC) clean


tools:
	$(MAKE) -C $(SRC) install

fsts:$(FSTS)

all-results:all-results-fi

all-results-fi:$(TRAINING_DATA_SIZES:%=$(FI_RESULTS)/result.%.txt) $(TRAINING_DATA_SIZES:%=$(FI_RESULTS)/result.kpc.%.txt)

$(FST_BIN)/text_to_numbers.hfst:$(FST_SRC)/numbers_to_text.txt
	cat $^ | hfst-txt2fst | hfst-invert > $@

$(FST_BIN)/combinatorics.hfst:$(FST_SRC)/combinatorics.twolc
	hfst-twolc -i $^ > $@

training_data:$(TRAINING_DATA_SIZES:%=$(FI_PROCESSED_DATA)/segmented_training_data.%.txt) $(TRAINING_DATA_SIZES:%=$(FI_PROCESSED_DATA)/letter_training_data.%.txt)

$(FI_PROCESSED_DATA)/segmented_training_data.%.txt:$(FI_DATA)/asplex.%.txt
	$(BIN)/morfessor1.0.perl -data $^ | egrep -v "^#" | \
	$(BIN)/rewrite_morfessor_output.pl | sed 's/^/</' | \
	sed 's/$$/>/' | sed 's/$$/\n||\n||/' | sed 's/ + />\n</g' | \
	sed 's/<\(.*\)>/\1	<\1>/' | sed 's/||/||	||/' > temp
	echo "||\t||\n||\t||" > header
	cat header temp > $@
	rm temp

$(FI_PROCESSED_DATA)/letter_training_data.%.txt:$(FI_DATA)/asplex.%.txt
	cat $< | sed 's/$$/\n||\n||/' | sed 's/\([a-zåäö]\)/\1\n/g' | \
	egrep -v "^$$" > temp1
	cat $< | hfst-lookup $(FST_BIN)/text_to_numbers.hfst | cut -f2 | \
	sed 's/^$$/||\n||/' | sed 's/\([0-9]\)/\1\n/g' | egrep -v "^$$"  \
        > temp2
	paste temp2 temp1 > temp3
	echo "||\t||\n||\t||" > header
	cat header temp3 > $@
	rm temp1 temp2 temp3 header

$(MODELS)/hmm_model_%.lex:$(FI_PROCESSED_DATA)/letter_training_data.%.txt
	$(BIN)/train_hmm_4_tagger $< $(MODELS)/hmm_model_$*

$(MODELS)/sequencer.%.hfst:$(FI_PROCESSED_DATA)/segmented_training_data.%.txt
	$(BIN)/train_sequencer $< $@

$(MODELS)/morpheme_sequence_model.%.hfst:$(FI_PROCESSED_DATA)/segmented_training_data.%.txt
	$(BIN)/train_morpheme_sequence_model $< $@

$(FI_RESULTS)/result.%.txt:$(MODELS)/hmm_model_%.lex $(MODELS)/sequencer.%.hfst $(MODELS)/morpheme_sequence_model.%.hfst $(FST_BIN)/text_to_numbers.hfst $(FST_BIN)/combinatorics.hfst $(FI_DATA)/test_data
	cat $(FI_DATA)/test_data | $(BIN)/t9_suggest $(MODELS)/hmm_model_$*.lex $(MODELS)/hmm_model_$*.seq $(MODELS)/sequencer.$*.hfst $(MODELS)/morpheme_sequence_model.$*.hfst $(FST_BIN)/text_to_numbers.hfst $(FST_BIN)/combinatorics.hfst > $@

$(FI_RESULTS)/result.kpc.%.txt:$(MODELS)/hmm_model_%.lex $(MODELS)/sequencer.%.hfst $(MODELS)/morpheme_sequence_model.%.hfst $(FST_BIN)/text_to_numbers.hfst $(FST_BIN)/combinatorics.hfst $(FI_DATA)/test_data
	cat $(FI_DATA)/test_data | $(BIN)/t9_suggest_kpc $(MODELS)/hmm_model_$*.lex $(MODELS)/hmm_model_$*.seq $(MODELS)/sequencer.$*.hfst $(MODELS)/morpheme_sequence_model.$*.hfst $(FST_BIN)/text_to_numbers.hfst $(FST_BIN)/combinatorics.hfst > $@