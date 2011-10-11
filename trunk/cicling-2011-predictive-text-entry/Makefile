BIN=bin
FSTS=fsts
SRC=src
FST_SRC=fst_src
FST_BIN=fsts
FI_DATA=data/finnish
FI_PROCESSED_DATA=processed_training_data/finnish
MODELS=models

FSTS=$(FST_BIN)/text_to_numbers.hfst $(FST_BIN)/combinatorics.hfst

all:tools fsts

clean:
	-rm -f $(BIN)/*
	-rm -f $(FST_BIN)/*
	-rm -f $(FI_PROCESSED_DATA)/*
	$(MAKE) -C $(SRC) clean


tools:
	$(MAKE) -C $(SRC) install

fsts:$(FSTS)

$(FST_BIN)/text_to_numbers.hfst:$(FST_SRC)/numbers_to_text.txt
	cat $^ | hfst-txt2fst | hfst-invert > $@

$(FST_BIN)/combinatorics.hfst:$(FST_SRC)/combinatorics.twolc
	hfst-twolc -i $^ > $@

$(FI_PROCESSED_DATA)/segmented_training_data.%.txt:$(FI_DATA)/asplex.%.txt
	$(BIN)/morfessor1.0.perl -data $^ | egrep -v "^#" | \
	$(BIN)/rewrite_morfessor_output.pl > $@

$(FI_PROCESSED_DATA)/letter_training_data.%.txt:$(FI_DATA)/asplex.%.txt
	cat $< | sed 's/$$/\n||\n||/' | sed 's/\([a-zåäö]\)/\1\n/g' | \
	egrep -v "^$$" > temp1
	cat $< | hfst-lookup $(FST_BIN)/text_to_numbers.hfst | cut -f2 | \
	sed 's/^$$/||\n||/' | sed 's/\([0-9]\)/\1\n/g' | egrep -v "^$$"  \
        > temp2
	paste temp1 temp2 > temp3
	echo "||\t||\n||\t||" > header
	cat header temp3 > $@
	rm temp1 temp2 temp3 header