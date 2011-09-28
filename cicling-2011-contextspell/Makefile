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

# finnish automata
# make these from different runs of omorfi or whatever; e.g.
# configure options --enable-tokenweights --enable-tagweights
$(OMORFI)/dictionary.default.unw.hfst: $(sharedir)/hfst/fi/dictionary.default.hfstol
	cp -v $< $@

$(OMORFI)/dictionary.default.tokenweights-no-boundaries.hfst: $(sharedir)/hfst/fi/dictionary.default.hfstol
	cp -v $< $@

$(OMORFI)/dictionary.default.tokenweights-boundary-maxw.hfst: $(sharedir)/hfst/fi/dictionary.default.hfstol
	cp -v $< $@

$(OMORFI)/dictionary.limited.hfst: $(sharedir)/hfst/fi/dictionary.filtered.hfstol
	cp -v $< $@

# 6.90878 = -log(1/1000) i.e. one typo per 1000 words
$(OMORFI)/error.edit-distance-2.hfst: 
	./edit-distance.bash 2 6.9078 a b c d e f g h i j k l m n o p q r s š t u v w x y z ž å ä ö |\
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
results/fi-FI.fiwiki.dictionary-full-unweighted.edit-distance-2.unigrams: fi-FI.fiwiki-gutenberg.contextspell $(OMORFI)/dictionary.default.unw.hfst $(OMORFI)/error.edit-distance-2.hfst
	hfst-ospell-cicling $(OMORFI)/error.edit-distance-2.hfst $(OMORFI)/dictionary.default.unw.hfst < fi-FI.fiwiki-gutenberg.contextspell > $@

results/en-US.norvig1.dictionary-norvig.edit-distance-2.unigrams: norvig1.contextspell $(NORVIG)/dictionary.default.hfst $(NORVIG)/error.edit-distance-2.hfst
	hfst-ospell-cicling $(NORVIG)/error.edit-distance-2.hfst $(NORVIG)/dictionary.default.hfst < norvig1.contextspell > $@

results/en-US.norvig2.dictionary-norvig.edit-distance-2.unigrams: norvig2.contextspell $(NORVIG)/dictionary.default.hfst $(NORVIG)/error.edit-distance-2.hfst
	hfst-ospell-cicling $(NORVIG)/error.edit-distance-2.hfst $(NORVIG)/dictionary.default.hfst < norvig2.contextspell > $@

clean:
	-rm -f cicling-2011-contextspell.aux cicling-2011-contextspell.bbl cicling-2011-contextspell.pdf words train tropical
