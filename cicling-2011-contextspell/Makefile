## Use make to create article pdfs or recreate test results

TEX=xelatex
BIB=bibtex

cicling-2011-contextspell.pdf: cicling-2011-contextspell.tex cicling2011.bib
	$(TEX) cicling-2011-contextspell.tex
	$(BIB) cicling-2011-contextspell
	$(TEX) cicling-2011-contextspell.tex
	$(TEX) cicling-2011-contextspell.tex
	$(TEX) cicling-2011-contextspell.tex

# automata building rules
OMORFI=hfst/fi
NORVIG=hfst/en

prefix=/usr/local
sharedir=$(prefix)/share

$
$(OMORFI):
	mkdir -p $@

$(NORVIG):
	mkdir -p $@

.PHONY: automata automata-fi automata-en results results-fi results-en textables textables-fi textables-en

automata: automata-fi automata-en

automata-fi: $(OMORFI)/dictionary.limited.hfst $(OMORFI)/dictionary.default-boundaryweight-medium.hfst $(OMORFI)/dictionary.default-boundaryweight-small.hfst $(OMORFI)/dictionary.default-boundaryweight-huge.hfst $(OMORFI)/dictionary.default-boundaryweight-none.hfst $(OMORFI)/error.edit-distance-2-medium.hfst $(OMORFI)/error.edit-distance-2-small.hfst $(OMORFI)/error.edit-distance-2-none.hfst $(OMORFI)/error.edit-distance-2-huge.hfst

automata-en: $(NORVIG)/dictionary.default.hfst $(NORVIG)/edit-distance-2.hfst

# finnish automata; gather dictionares from omorfi
# NB: backoff at 5.25667 needs to be changed if other wiki is used
$(OMORFI)/dictionary.limited.hfst: $(sharedir)/hfst/fi/dictionary.filtered.hfst $(OMORFI)/dictionary.fiwiki.hfst
	hfst-fst2fst $(sharedir)/hfst/fi/dictionary.filtered.hfst -f openfst-tropical |\
		hfst-reweight -e -a 5.2667 |\
		hfst-union $(OMORFI)/dictionary.fiwiki.hfst |\
		hfst-minimize |\
		hfst-fst2fst -f  olw -o $@

# ensure current svn revision with boundary weighting
$(OMORFI)/dictionary.default-boundaryweight-medium.hfst: $(sharedir)/hfst/fi/dictionary.default.hfst $(OMORFI)/dictionary.fiwiki.hfst
	hfst-fst2fst $(sharedir)/hfst/fi/dictionary.default.hfst -f openfst-tropical |\
		hfst-reweight -e -a 5.2667 |\
		hfst-union $(OMORFI)/dictionary.fiwiki.hfst |\
		hfst-minimize |\
		hfst-fst2fst -f  olw -o $@

$(OMORFI)/dictionary.default-boundaryweight-small.hfst: $(sharedir)/hfst/fi/dictionary.default.hfst $(OMORFI)/dictionary.fiwiki.hfst
	hfst-fst2fst $(sharedir)/hfst/fi/dictionary.default.hfst -f openfst-tropical |\
		hfst-reweight -b 0.01 |\
		hfst-reweight -e -a 5.2667 |\
		hfst-union $(OMORFI)/dictionary.fiwiki.hfst |\
		hfst-minimize |\
		hfst-fst2fst -f  olw -o $@

$(OMORFI)/dictionary.default-boundaryweight-huge.hfst: $(sharedir)/hfst/fi/dictionary.default.hfst $(OMORFI)/dictionary.fiwiki.hfst
	hfst-fst2fst $(sharedir)/hfst/fi/dictionary.default.hfst -f openfst-tropical |\
		hfst-reweight -b 100 |\
		hfst-reweight -e -a 5.2667 |\
		hfst-union $(OMORFI)/dictionary.fiwiki.hfst |\
		hfst-minimize |\
		hfst-fst2fst -f  olw -o $@

$(OMORFI)/dictionary.default-boundaryweight-none.hfst: $(sharedir)/hfst/fi/dictionary.default.hfst $(OMORFI)/dictionary.fiwiki.hfst
	hfst-fst2fst $(sharedir)/hfst/fi/dictionary.default.hfst -f openfst-tropical |\
		hfst-reweight -b 0 |\
		hfst-reweight -e -a 5.2667 |\
		hfst-union $(OMORFI)/dictionary.fiwiki.hfst |\
		hfst-minimize |\
		hfst-fst2fst -f  olw -o $@

$(OMORFI)/dictionary.fiwiki.hfst: fiwiki.strings
	sort < fiwiki.strings | uniq -c | sort -nr > train
	awk -f tropicalize-uniq-c.awk --assign CS=`wc -l < fiwiki.strings` < train > tropical
	hfst-strings2fst -j -f openfst-tropical tropical -v |\
		hfst-minimize -v -o $@


# 6.90878 = -log(1/1000) i.e. one typo per 1000 words
$(OMORFI)/error.edit-distance-2-medium.hfst:  edit-distance.bash
	./edit-distance.bash 2 6.9078 a b c d e f g h i j k l m n o p q r s š t u v w x y z ž å ä ö |\
		hfst-txt2fst -v -f openfst-tropical |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

$(OMORFI)/error.edit-distance-2-small.hfst:  edit-distance.bash
	./edit-distance.bash 2 0.69078 a b c d e f g h i j k l m n o p q r s š t u v w x y z ž å ä ö |\
		hfst-txt2fst -v -f openfst-tropical |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

$(OMORFI)/error.edit-distance-2-none.hfst:  edit-distance.bash
	./edit-distance.bash 2 0.0 a b c d e f g h i j k l m n o p q r s š t u v w x y z ž å ä ö |\
		hfst-txt2fst -v -f openfst-tropical |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

$(OMORFI)/error.edit-distance-2-huge.hfst: edit-distance.bash
	./edit-distance.bash 2 690.78 a b c d e f g h i j k l m n o p q r s š t u v w x y z ž å ä ö |\
		hfst-txt2fst -v -f openfst-tropical |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

# english automata
# norvig's statistic dictionary
big.txt:
	wget http://norvig.com/big.txt

$(NORVIG)/dictionary.default.hfst: big.txt tropicalize-uniq-c.awk
	tr -s '[:punct:][:digit:]' ' ' < $< | tr -s '[:space:]' '\n' | tr '[:upper:]' '[:lower:]' > words
	sort < words | uniq -c | sort -nr > train
	awk -f tropicalize-uniq-c.awk --assign CS=`wc -l < words` < train > tropical
	hfst-strings2fst -j -f openfst-tropical tropical -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

# 6.90878 = -log(1/1000) i.e. one typo per 1000 words
$(NORVIG)/error.edit-distance-2.hfst: edit-distance.bash 
	./edit-distance.bash 2 6.9078 a b c d e f g h i j k l m n o p q r s t u v w x y z |\
		hfst-txt2fst -v -f openfst-tropical |\
		hfst-minimize -v  |\
		hfst-fst2fst -v -f olw -o $@

# automata testing rules
results: results-fi results-en

results-fi: results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-none.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-none.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-none.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-none.unigrams  results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-small.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-small.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-small.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-small.unigrams  results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-medium.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-medium.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-medium.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-medium.unigrams  results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-huge.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-huge.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-huge.unigrams results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-huge.unigrams 

results-en: results/en-US.norvig1.dictionary-norvig.edit-distance-2.unigrams results/en-US.norvig2.dictionary-norvig.edit-distance-2.unigrams 

textables: textables-fi textables-en

textables-fi: results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-none.tex results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-none.tex results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-none.tex results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-none.tex  results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-small.tex results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-small.tex results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-small.tex results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-small.tex  results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-medium.tex results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-medium.tex results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-medium.tex results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-medium.tex  results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-huge.tex results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-huge.tex results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-huge.tex results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-huge.tex 

textables-en: results/en-US.norvig1.dictionary-norvig.edit-distance-2.tex results/en-US.norvig2.dictionary-norvig.edit-distance-2.tex

results/%.tex: results/%.unigrams
	sed -e 's/\[[^]]*\]//g' < $< | awk -f results2tex.awk > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-none.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-none.hfst $(OMORFI)/error.edit-distance-2-none.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-none.hfst $(OMORFI)/dictionary.default-boundaryweight-none.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-none.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-small.hfst $(OMORFI)/error.edit-distance-2-none.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-none.hfst $(OMORFI)/dictionary.default-boundaryweight-small.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-none.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-medium.hfst $(OMORFI)/error.edit-distance-2-none.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-none.hfst $(OMORFI)/dictionary.default-boundaryweight-medium.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-none.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-huge.hfst $(OMORFI)/error.edit-distance-2-none.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-none.hfst $(OMORFI)/dictionary.default-boundaryweight-huge.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-small.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-none.hfst $(OMORFI)/error.edit-distance-2-small.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-small.hfst $(OMORFI)/dictionary.default-boundaryweight-none.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-small.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-small.hfst $(OMORFI)/error.edit-distance-2-small.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-small.hfst $(OMORFI)/dictionary.default-boundaryweight-small.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-small.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-medium.hfst $(OMORFI)/error.edit-distance-2-small.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-medium.hfst $(OMORFI)/dictionary.default-boundaryweight-small.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-small.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-huge.hfst $(OMORFI)/error.edit-distance-2-small.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-small.hfst $(OMORFI)/dictionary.default-boundaryweight-huge.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-medium.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-none.hfst $(OMORFI)/error.edit-distance-2-medium.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-medium.hfst $(OMORFI)/dictionary.default-boundaryweight-none.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-medium.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-small.hfst $(OMORFI)/error.edit-distance-2-medium.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-medium.hfst $(OMORFI)/dictionary.default-boundaryweight-small.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-medium.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-medium.hfst $(OMORFI)/error.edit-distance-2-medium.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-medium.hfst $(OMORFI)/dictionary.default-boundaryweight-medium.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-medium.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-huge.hfst $(OMORFI)/error.edit-distance-2-medium.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-medium.hfst $(OMORFI)/dictionary.default-boundaryweight-huge.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-none.edit-distance-2-huge.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-none.hfst $(OMORFI)/error.edit-distance-2-huge.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-huge.hfst $(OMORFI)/dictionary.default-boundaryweight-medium.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-small.edit-distance-2-huge.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-small.hfst $(OMORFI)/error.edit-distance-2-huge.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-huge.hfst $(OMORFI)/dictionary.default-boundaryweight-small.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-medium.edit-distance-2-huge.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-medium.hfst $(OMORFI)/error.edit-distance-2-huge.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-huge.hfst $(OMORFI)/dictionary.default-boundaryweight-medium.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/fi-FI.fiwiki.dictionary-full-boundaries-huge.edit-distance-2-huge.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default-boundaryweight-huge.hfst $(OMORFI)/error.edit-distance-2-huge.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2-huge.hfst $(OMORFI)/dictionary.default-boundaryweight-huge.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/en-US.norvig1.dictionary-norvig.edit-distance-2.unigrams: norvig1.contextspell $(NORVIG)/dictionary.default.hfst $(NORVIG)/error.edit-distance-2.hfst
	hfst-ospell-cicling $(NORVIG)/error.edit-distance-2.hfst $(NORVIG)/dictionary.default.hfst < norvig1.contextspell > $@

results/en-US.norvig2.dictionary-norvig.edit-distance-2.unigrams: norvig2.contextspell $(NORVIG)/dictionary.default.hfst $(NORVIG)/error.edit-distance-2.hfst
	hfst-ospell-cicling $(NORVIG)/error.edit-distance-2.hfst $(NORVIG)/dictionary.default.hfst < norvig2.contextspell > $@

clean:
	-rm -f cicling-2011-contextspell.aux cicling-2011-contextspell.bbl cicling-2011-contextspell.pdf words train tropical
