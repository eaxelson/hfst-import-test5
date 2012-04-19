# Execute this Makefile with GNU compatible make to recreate the test results
# and paper for article titled
# `blah' in Proceedings of FSMLNP 2012

# point programs to right binaries
PYTHON=python3
HFST_EDIT_DISTANCE=hfst-edit-distance
HFST_OSPELL=hfst-ospell
HFST_FST2FST=hfst-fst2fst
XELATEX=pdflatex
BIBTEX=bibtex
WGET=wget
BZCAT=bzcat

HUNSPELL=hunspell -1 -a
VOIKKOSPELL=voikkospell -s

EN_DICT=hfst/en/norvig.hfst
FI_DICT=hfst/fi/omorfi-20110505.hfst
KL_DICT=hfst/kl/kl.hfst

TESTS=1 2 3 4 5
WORDS=10000

# article
fsmnlp-2012-speedspelling.pdf: fsmnlp-2012-speedspelling.tex fsmnlp2012.bib
	$(XELATEX) fsmnlp-2012-speedspelling.tex
	$(BIBTEX)  fsmnlp-2012-speedspelling
	$(XELATEX) fsmnlp-2012-speedspelling.tex
	$(XELATEX) fsmnlp-2012-speedspelling.tex
	$(XELATEX) fsmnlp-2012-speedspelling.tex

results: corpora dictionaries models tables
corpora: corp/fi/fiwiki-20120213-pages-articles.2000000.xml \
		 corp/en/enwiki-20120211-pages-articles.2000000.xml \
		 corp/kl/klwiki-20120214-pages-articles.xml \
		 corp/en/enwiki.$(WORDS).words \
		 corp/fi/fiwiki.$(WORDS).words \
		 corp/kl/klwiki.$(WORDS).words \
		 corp/en/automisp.words \
		 corp/fi/automisp.words \
		 corp/kl/automisp.words
dictionaries: hfst/en/norvig.hfst \
			  hfst/fi/omorfi-20110505.hfst \
			  hfst/kl/kl.hfst
models: hfst/fi/errmodel.edit-distance-1.hfst \
		hfst/en/errmodel.edit-distance-1.hfst \
		hfst/kl/errmodel.edit-distance-1.hfst \
		hfst/en/errmodel.edit-distance-2.hfst \
		hfst/fi/errmodel.edit-distance-2.hfst \
		hfst/kl/errmodel.edit-distance-2.hfst \
		hfst/en/errmodel.edit-distance-3.hfst \
		hfst/fi/errmodel.edit-distance-3.hfst \
		hfst/kl/errmodel.edit-distance-3.hfst \
		hfst/en/errmodel.edit-distance-2-no-redundant.hfst \
		hfst/fi/errmodel.edit-distance-2-no-redundant.hfst \
		hfst/kl/errmodel.edit-distance-2-no-redundant.hfst \
		hfst/en/errmodel.edit-distance-3-no-redundant.hfst \
		hfst/fi/errmodel.edit-distance-3-no-redundant.hfst \
		hfst/kl/errmodel.edit-distance-3-no-redundant.hfst \
		hfst/fi/errmodel.edit-distance-1-no-initial.hfst \
		hfst/en/errmodel.edit-distance-1-no-initial.hfst \
		hfst/kl/errmodel.edit-distance-1-no-initial.hfst \
		hfst/en/errmodel.edit-distance-2-no-initial.hfst \
		hfst/fi/errmodel.edit-distance-2-no-initial.hfst \
		hfst/kl/errmodel.edit-distance-2-no-initial.hfst \
		hfst/en/errmodel.edit-distance-3-no-initial.hfst \
		hfst/fi/errmodel.edit-distance-3-no-initial.hfst \
		hfst/kl/errmodel.edit-distance-3-no-initial.hfst \
		hfst/en/errmodel.edit-distance-2-no-initial-no-redundant.hfst \
		hfst/fi/errmodel.edit-distance-2-no-initial-no-redundant.hfst \
		hfst/kl/errmodel.edit-distance-2-no-initial-no-redundant.hfst \
		hfst/en/errmodel.edit-distance-3-no-initial-no-redundant.hfst \
		hfst/fi/errmodel.edit-distance-3-no-initial-no-redundant.hfst \
		hfst/kl/errmodel.edit-distance-3-no-initial-no-redundant.hfst


tables:results/table1.tex results/table2.tex \
	results/table3.tex results/table4.tex results/table5.tex

clean-models:
	rm hfst/fi/errmodel.edit-distance-1.hfst \
	hfst/en/errmodel.edit-distance-1.hfst \
	hfst/kl/errmodel.edit-distance-1.hfst

# tests and tables...
#
# corpora acquisition
fiwiki-20120213-pages-articles.xml.bz2:
	$(WGET) 'http://dumps.wikimedia.org/fiwiki/20120213/fiwiki-20120213-pages-articles.xml.bz2'

corp/fi/fiwiki-20120213-pages-articles.2000000.xml: fiwiki-20120213-pages-articles.xml.bz2
	$(BZCAT) $< | head -n 2000000 > $@

enwiki-20120211-pages-articles.xml.bz2:
	$(WGET) 'http://dumps.wikimedia.org/enwiki/20120211/enwiki-20120211-pages-articles.xml.bz2'

corp/en/enwiki-20120211-pages-articles.2000000.xml: enwiki-20120211-pages-articles.xml.bz2
	$(BZCAT) $< | head -n 2000000 > $@

klwiki-20120214-pages-articles.xml.bz2:
	$(WGET) 'http://dumps.wikimedia.org/klwiki/20120214/klwiki-20120214-pages-articles.xml.bz2'

corp/kl/klwiki-20120214-pages-articles.xml: klwiki-20120214-pages-articles.xml.bz2
	$(BZCAT) $< > $@

# corpora preprocessing (ugly fixme plz)
corp/en/enwiki.$(WORDS).words: corp/en/enwiki-20120211-pages-articles.2000000.xml wikipedia-deform.sh
	awk '/<text/,/<\/text/ {print}' < $< |\
		./wikipedia-deform.sh |\
		tr -s ' ' '\n' |\
		head -n $(WORDS) > $@

corp/fi/fiwiki.$(WORDS).words: corp/fi/fiwiki-20120213-pages-articles.2000000.xml
	awk '/<text/,/<\/text/ {print}' < $< |\
		./wikipedia-deform.sh |\
		tr -s ' ' '\n' |\
		head -n $(WORDS) > $@

corp/kl/klwiki.$(WORDS).words: corp/kl/klwiki-20120214-pages-articles.xml
	awk '/<text/,/<\/text/ {print}' < $< |\
		./wikipedia-deform.sh |\
		tr -s ' ' '\n' |\
		head -n $(WORDS) > $@

# to 1:1 recreate article results please be using --seed=15632753
corp/en/automisp.words: corp/en/enwiki.$(WORDS).words
	python lines2misspell.py --seed=15632753 --input=$< --output=$@

corp/fi/automisp.words: corp/fi/fiwiki.$(WORDS).words
	python lines2misspell.py --seed=15632753 --input=$< --output=$@

corp/kl/automisp.words: corp/kl/klwiki.$(WORDS).words
	python lines2misspell.py --seed=15632753 --input=$< --output=$@

# fetching / concocting dictionary automata from external sources
big.txt:
	wget http://norvig.com/big.txt

hfst/en/norvig.hfst: big.txt
	tr -s '[.:?!;:-][:digit:]' ' ' < big.txt | tr -s '[:space:]' '\n' | tr '[:upper:]' '[:lower:]' > words
	sort < words | uniq -c | sort -nr > train
	awk -f tropicalize-uniq-c.awk --assign CS=`wc -l < words` < train > tropical
	hfst-strings2fst -j -f openfst-tropical -v < tropical |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/fi/omorfi-20110505.hfst:
	$(WGET) 'http://download.gna.org/omorfi/omorfi-20110505-transducers.tar.xz'
	tar Jxvf omorfi-20110505-transducers.tar.xz
	mv omorfi-transducers-20110505/dictionary.default.hfstol $@

hfst/kl/kl.hfst:
	wget 'http://sourceforge.net/projects/hfst/files/spell-transducers/voikko-kl-linux-x86_64-static.oxt/download' -O voikko-kl-linux-x86_64-static.oxt
	unzip voikko-kl-linux-x86_64-static.oxt
	unzip 2/mor-hfst-kl/speller.zhfst
	mv acceptor.default.hfst $@

# create all error models here

# Basic ED 1
hfst/fi/errmodel.edit-distance-1.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim -d 1 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-1.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim -d 1 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-1.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim -d 1 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@
# Basic ED 2
hfst/fi/errmodel.edit-distance-2.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim -d 2 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-2.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim -d 2 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-2.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim -d 2 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@
# Basic ED 3
hfst/fi/errmodel.edit-distance-3.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim -d 3 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-3.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim -d 3 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-3.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim -d 3 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 2, eliminate redundancies
hfst/fi/errmodel.edit-distance-2-no-redundant.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s -d 2 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-2-no-redundant.hfst: hfst/en/norvig.hfst
	python editdist.py -s -d 2 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-2-no-redundant.hfst: hfst/kl/kl.hfst
	python editdist.py -s -d 2 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 3, eliminate redundancies
hfst/fi/errmodel.edit-distance-3-no-redundant.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s -d 3 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-3-no-redundant.hfst: hfst/en/norvig.hfst
	python editdist.py -s -d 3 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-3-no-redundant.hfst: hfst/kl/kl.hfst
	python editdist.py -s -d 3 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 1, no initial edit
hfst/fi/errmodel.edit-distance-1-no-initial.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 1 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-1-no-initial.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 1 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-1-no-initial.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 1 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 2, no initial edit
hfst/fi/errmodel.edit-distance-2-no-initial.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 2 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-2-no-initial.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 2 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-2-no-initial.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 2 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 3, no initial edit
hfst/fi/errmodel.edit-distance-3-no-initial.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 3 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-3-no-initial.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 3 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-3-no-initial.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 3 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 2, no initial edit, eliminate redundancies
hfst/fi/errmodel.edit-distance-2-no-initial-no-redundant.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-string-initial-correction -d 2 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-2-no-initial-no-redundant.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-string-initial-correction -d 2 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-2-no-initial-no-redundant.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-string-initial-correction -d 2 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 3, no initial edit
hfst/fi/errmodel.edit-distance-3-no-initial-no-redundant.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-string-initial-correction -d 3 abcdefghijklmnopqrstuvwxyzåäöABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-3-no-initial-no-redundant.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-string-initial-correction -d 3 abcdefghijklmnopqrstuvwxyz |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-3-no-initial-no-redundant.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-string-initial-correction -d 3 abcdefghijklmnopqrstuvwxyzåäæøöABCDEFGHIJKLMNOPQRSTUVWXYZÅÆØÄÖ |\
		hfst-txt2fst -v |\
		hfst-fst2fst -v -f olw -o $@


# (re)generate results in neat tables
# combine all bits and pieces for table 1:
TABLE1_DATA=\
			results/en/table1.dict-hunspell.errm-hunspell.corp-wiki.tex \
			results/fi/table1.dict-voikko.errm-voikko.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed1.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed1.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed1.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed2.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed2.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed2.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed3.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed3.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed3.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed1-no-initial.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed1-no-initial.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed1-no-initial.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed2-no-initial.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed2-no-initial.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed2-no-initial.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed3-no-initial.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed3-no-initial.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed3-no-initial.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed2-no-redundant.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed2-no-redundant.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed2-no-redundant.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed3-no-redundant.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed3-no-redundant.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed3-no-redundant.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-fallback.corp-wiki.tex \
			results/fi/table1.dict-omorfi.errm-fallback.corp-wiki.tex \
			results/kl/table1.dict-kl.errm-fallback.corp-wiki.tex \
			results/en/table1.dict-norvig.errm-ed2.corp-wiki.generate-all.tex \
			results/fi/table1.dict-omorfi.errm-ed2.corp-wiki.generate-all.tex \
			results/kl/table1.dict-kl.errm-ed2.corp-wiki.generate-all.tex
# 			results/en/table1.dict-norvig.errm-ed3-no-redundant.corp-wiki.tex \
# 			results/fi/table1.dict-omorfi.errm-ed3-no-redundant.corp-wiki.tex \
# 			results/kl/table1.dict-kl.errm-ed3-no-redundant.corp-wiki.tex \
# 			results/en/table1.dict-norvig.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
# 			results/fi/table1.dict-omorfi.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
# 			results/kl/table1.dict-kl.errm-ed3-no-initial-no-redundant.corp-wiki.tex \



results/table1.tex: $(TABLE1_DATA)
	echo "Reference impl. &" > $@
	paste -d '&' results/en/table1.dict-hunspell.errm-hunspell.corp-wiki.tex \
		results/fi/table1.dict-voikko.errm-voikko.corp-wiki.tex >> $@
	echo "& --- & en: hunspell, fi: voikko, kl: ---  \\\\\\\\" >> $@
	echo "Generate all edits 2 & " >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed2.corp-wiki.generate-all.tex \
		results/fi/table1.dict-omorfi.errm-ed2.corp-wiki.generate-all.tex \
		results/kl/table1.dict-kl.errm-ed2.corp-wiki.generate-all.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "\\hline" >> $@
	echo "Edit distance 1 &" >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed1.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-ed1.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-ed1.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "Edit distance 2 &" >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed2.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-ed2.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-ed2.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "Edit distance 3 &" >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed3.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-ed3.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-ed3.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "No first edits ed 1 & " >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed1-no-initial.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-ed1-no-initial.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-ed1-no-initial.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "No first edits ed 2 &" >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed2-no-initial.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-ed2-no-initial.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-ed2-no-initial.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "No first edits ed 3 &" >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed3-no-initial.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-ed3-no-initial.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-ed3-no-initial.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "Avoid redundancy and first edits ed 2 &" >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-ed2-no-initial-no-redundant.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "Avoid redundancy and first edits ed 3 &" >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-ed3-no-initial-no-redundant.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "Lower order first ed 1 to 2 &" >> $@
	paste -d '&' results/en/table1.dict-norvig.errm-fallback.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-fallback.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-fallback.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@


# Table 1, ED 1

results/en/table1.dict-hunspell.errm-hunspell.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.times
	for i in $(TESTS) ; do \
		/usr/bin/time -f %U -a -o $@.times \
			$(HUNSPELL) -d en_US corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f timelist2tex.awk < $@.times > $@

results/en/table1.dict-norvig.errm-ed1.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-1.hfst $(EN_DICT) < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-voikko.errm-voikko.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.times
	for i in $(TESTS) ; do \
		/usr/bin/time -f %U -a -o $@.times \
			$(VOIKKOSPELL) -d fi_FI < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f timelist2tex.awk < $@.times > $@

results/fi/table1.dict-omorfi.errm-ed1.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-1.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed1.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-1.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 2

results/en/table1.dict-norvig.errm-ed2.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2.hfst $(EN_DICT) < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-ed2.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed2.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 3

results/en/table1.dict-norvig.errm-ed3.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-3.hfst $(EN_DICT) < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-ed3.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-3.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed3.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-3.hfst hfst/kl/kl.hfst < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 1, no initial

results/en/table1.dict-norvig.errm-ed1-no-initial.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-1-no-initial.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-ed1-no-initial.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-1-no-initial.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed1-no-initial.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-1-no-initial.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 2, no initial

results/en/table1.dict-norvig.errm-ed2-no-initial.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2-no-initial.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-ed2-no-initial.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2-no-initial.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed2-no-initial.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2-no-initial.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 3, no initial

results/en/table1.dict-norvig.errm-ed3-no-initial.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-3-no-initial.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-ed3-no-initial.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-3-no-initial.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed3-no-initial.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-3-no-initial.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 2, eliminate redundancies

results/en/table1.dict-norvig.errm-ed2-no-redundant.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-ed2-no-redundant.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed2-no-redundant.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 3, eliminate redundancies

results/en/table1.dict-norvig.errm-ed3-no-redundant.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-3.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-ed3-no-redundant.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	#for i in $(TESTS) ; do \
	#	hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-3-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	#done
	#awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed3-no-redundant.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	#for i in $(TESTS) ; do \
	#	hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-3-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	#done
	#awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 2, no first and no redundancies

results/en/table1.dict-norvig.errm-ed2-no-initial-no-redundant.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2-no-initial-no-redundant.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-ed2-no-initial-no-redundant.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2-no-initial-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed2-no-initial-no-redundant.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2-no-initial-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 3, no first and no redundancies

results/en/table1.dict-norvig.errm-ed3-no-initial-no-redundant.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-3-no-initial-no-redundant.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-ed3-no-initial-no-redundant.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-3-no-initial-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-ed3-no-initial-no-redundant.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-3-no-initial-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, ED 2, fallback from ED 1 to ED 2 (no redundancies)

results/en/table1.dict-norvig.errm-fallback.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-1.hfst hfst/en/errmodel.edit-distance-2-no-redundant.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table1.dict-omorfi.errm-fallback.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-1.hfst hfst/fi/errmodel.edit-distance-2-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table1.dict-kl.errm-fallback.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-1.hfst hfst/kl/errmodel.edit-distance-2-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Table 1, generate all edits first for ED2

results/en/table1.dict-norvig.errm-ed2.corp-wiki.generate-all.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.time
	for i in $(TESTS) ; do \
		head -n 100 < corp/en/enwiki.$(WORDS).words |\
		/usr/bin/time -f %U -a -o $@.time \
		hfst-optimized-lookup hfst/en/errmodel.edit-distance-2.hfst \
		| cut -f 2  |\
		/usr/bin/time -f %U -a -o $@.time \
		hfst-optimized-lookup hfst/en/norvig.hfst > /dev/null ; done
	awk -f timelist2tex.awk < $@.time > $@

results/fi/table1.dict-omorfi.errm-ed2.corp-wiki.generate-all.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.time
	for i in $(TESTS) ; do \
		head -n 100 < corp/fi/fiwiki.$(WORDS).words |\
		/usr/bin/time -f %U -a -o $@.time \
		hfst-optimized-lookup hfst/fi/errmodel.edit-distance-2.hfst \
		| cut -f 2  |\
		/usr/bin/time -f %U -a -o $@.time \
		hfst-optimized-lookup $(FI_DICT) > /dev/null ; done
	awk -f timelist2tex.awk < $@.time > $@

results/kl/table1.dict-kl.errm-ed2.corp-wiki.generate-all.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.time
	for i in $(TESTS) ; do \
		head -n 100 < corp/kl/klwiki.$(WORDS).words |\
		/usr/bin/time -f %U -a -o $@.time \
		hfst-optimized-lookup hfst/kl/errmodel.edit-distance-2.hfst \
		| cut -f 2  |\
		/usr/bin/time -f %U -a -o $@.time \
		hfst-optimized-lookup $(KL_DICT) > /dev/null ; done
	awk -f timelist2tex.awk < $@.time > $@

# Table 2 (memory)

TABLE2_DATA=\
			results/en/table2.dict-norvig.errm-ed1.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed1.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed1.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-ed2.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed2.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed2.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-ed3.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed3.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed3.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-ed1-no-initial.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed1-no-initial.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed1-no-initial.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-ed2-no-initial.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed2-no-initial.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed2-no-initial.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-ed3-no-initial.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed3-no-initial.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed3-no-initial.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-ed2-no-redundant.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed2-no-redundant.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed2-no-redundant.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-ed3-no-redundant.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed3-no-redundant.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed3-no-redundant.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
			results/en/table2.dict-norvig.errm-fallback.corp-wiki.tex \
			results/fi/table2.dict-omorfi.errm-fallback.corp-wiki.tex \
			results/kl/table2.dict-kl.errm-fallback.corp-wiki.tex

results/table2.tex: $(TABLE2_DATA)
	#echo "Reference impl. &" > $@
	#paste -d '&' results/en/table2.dict-hunspell.errm-hunspell.corp-wiki.tex \
	#	results/fi/table2.dict-voikko.errm-voikko.corp-wiki.tex >> $@
	#echo "? & ? & --- \\\\\\\\" >> $@
	#echo "Generate all edits 2 & " >> $@
	#paste -d '&' results/en/table2.dict-norvig.errm-ed2.corp-wiki.generate-all.tex \
	#	results/fi/table2.dict-omorfi.errm-ed2.corp-wiki.generate-all.tex \
	#	results/kl/table2.dict-kl.errm-ed2.corp-wiki.generate-all.tex >> $@
	#echo "\\\\\\\\" >> $@
	echo "\\hline" >> $@
	echo "Edit distance 1 &" >> $@
	paste -d '&' results/en/table2.dict-norvig.errm-ed1.corp-wiki.tex \
		results/fi/table2.dict-omorfi.errm-ed1.corp-wiki.tex \
		results/kl/table2.dict-kl.errm-ed1.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "Edit distance 2 &" >> $@
	paste -d '&' results/en/table2.dict-norvig.errm-ed2.corp-wiki.tex \
		results/fi/table2.dict-omorfi.errm-ed2.corp-wiki.tex \
		results/kl/table2.dict-kl.errm-ed2.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "Edit distance 3 &" >> $@
	paste -d '&' results/en/table2.dict-norvig.errm-ed3.corp-wiki.tex \
		results/fi/table2.dict-omorfi.errm-ed3.corp-wiki.tex \
		results/kl/table2.dict-kl.errm-ed3.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "No firsts ed 1 & " >> $@
	paste -d '&' results/en/table2.dict-norvig.errm-ed1-no-initial.corp-wiki.tex \
		results/fi/table2.dict-omorfi.errm-ed1-no-initial.corp-wiki.tex \
		results/kl/table2.dict-kl.errm-ed1-no-initial.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "No firsts ed 2 &" >> $@
	paste -d '&' results/en/table2.dict-norvig.errm-ed2-no-initial.corp-wiki.tex \
		results/fi/table2.dict-omorfi.errm-ed2-no-initial.corp-wiki.tex \
		results/kl/table2.dict-kl.errm-ed2-no-initial.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "No firsts ed 3 &" >> $@
	paste -d '&' results/en/table2.dict-norvig.errm-ed3-no-initial.corp-wiki.tex \
		results/fi/table2.dict-omorfi.errm-ed3-no-initial.corp-wiki.tex \
		results/kl/table2.dict-kl.errm-ed3-no-initial.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "No redundancy or firsts ed 2 &" >> $@
	paste -d '&' results/en/table2.dict-norvig.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
		results/fi/table2.dict-omorfi.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
		results/kl/table2.dict-kl.errm-ed2-no-initial-no-redundant.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "No redundancy or firsts ed 3 &" >> $@
	paste -d '&' results/en/table2.dict-norvig.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
		results/fi/table2.dict-omorfi.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
		results/kl/table2.dict-kl.errm-ed3-no-initial-no-redundant.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@
	echo "Lower order first ed 1 to 2 &" >> $@
	paste -d '&' results/en/table2.dict-norvig.errm-fallback.corp-wiki.tex \
		results/fi/table2.dict-omorfi.errm-fallback.corp-wiki.tex \
		results/kl/table2.dict-kl.errm-fallback.corp-wiki.tex >> $@
	echo "& \\\\\\\\" >> $@


# Table 2, ED 1

results/en/table2.dict-norvig.errm-ed1.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-1.hfst $(EN_DICT) < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed1.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-1.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed1.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-1.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 2

results/en/table2.dict-norvig.errm-ed2.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2.hfst $(EN_DICT) < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed2.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed2.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 3

results/en/table2.dict-norvig.errm-ed3.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-3.hfst $(EN_DICT) < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed3.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-3.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed3.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-3.hfst hfst/kl/kl.hfst < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 1, no initial

results/en/table2.dict-norvig.errm-ed1-no-initial.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-1-no-initial.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed1-no-initial.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-1-no-initial.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed1-no-initial.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-1-no-initial.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 2, no initial

results/en/table2.dict-norvig.errm-ed2-no-initial.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2-no-initial.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed2-no-initial.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2-no-initial.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed2-no-initial.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2-no-initial.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 3, no initial

results/en/table2.dict-norvig.errm-ed3-no-initial.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-3-no-initial.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed3-no-initial.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-3-no-initial.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed3-no-initial.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-3-no-initial.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 2, eliminate redundancies

results/en/table2.dict-norvig.errm-ed2-no-redundant.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed2-no-redundant.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed2-no-redundant.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 3, eliminate redundancies

results/en/table2.dict-norvig.errm-ed3-no-redundant.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-3.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed3-no-redundant.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	#for i in $(TESTS) ; do \
	#	hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-3-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	#done
	#awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed3-no-redundant.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	#for i in $(TESTS) ; do \
	#	hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-3-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	#done
	#awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 2, no first and no redundancies

results/en/table2.dict-norvig.errm-ed2-no-initial-no-redundant.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2-no-initial-no-redundant.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed2-no-initial-no-redundant.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2-no-initial-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed2-no-initial-no-redundant.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2-no-initial-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 3, no first and no redundancies

results/en/table2.dict-norvig.errm-ed3-no-initial-no-redundant.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-3-no-initial-no-redundant.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-ed3-no-initial-no-redundant.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-3-no-initial-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-ed3-no-initial-no-redundant.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-3-no-initial-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 2, ED 2, fallback from ED 1 to ED 2 (no redundancies)

results/en/table2.dict-norvig.errm-fallback.corp-wiki.tex: corp/en/enwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-1.hfst hfst/en/errmodel.edit-distance-2-no-redundant.hfst hfst/en/norvig.hfst < corp/en/enwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/fi/table2.dict-omorfi.errm-fallback.corp-wiki.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-1.hfst hfst/fi/errmodel.edit-distance-2-no-redundant.hfst $(FI_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

results/kl/table2.dict-kl.errm-fallback.corp-wiki.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-1.hfst hfst/kl/errmodel.edit-distance-2-no-redundant.hfst $(KL_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

# Table 3 (quality)

TABLE3_DATA=\
			results/en/table3.dict-norvig.errm-ed1.corp-wiki.tex\
			results/fi/table3.dict-omorfi.errm-ed1.corp-wiki.tex\
			results/kl/table3.dict-kl.errm-ed1.corp-wiki.tex \
			results/en/table3.dict-norvig.errm-ed2.corp-wiki.tex \
			results/fi/table3.dict-omorfi.errm-ed2.corp-wiki.tex \
			results/kl/table3.dict-kl.errm-ed2.corp-wiki.tex \
			results/en/table3.dict-norvig.errm-ed3.corp-wiki.tex \
			results/fi/table3.dict-omorfi.errm-ed3.corp-wiki.tex \
			results/kl/table3.dict-kl.errm-ed3.corp-wiki.tex \
			results/en/table3.dict-norvig.errm-ed1-no-initial.corp-wiki.tex \
			results/fi/table3.dict-omorfi.errm-ed1-no-initial.corp-wiki.tex \
			results/kl/table3.dict-kl.errm-ed1-no-initial.corp-wiki.tex \
			results/en/table3.dict-norvig.errm-ed2-no-initial.corp-wiki.tex \
			results/fi/table3.dict-omorfi.errm-ed2-no-initial.corp-wiki.tex \
			results/kl/table3.dict-kl.errm-ed2-no-initial.corp-wiki.tex \
			results/en/table3.dict-norvig.errm-ed3-no-initial.corp-wiki.tex \
			results/fi/table3.dict-omorfi.errm-ed3-no-initial.corp-wiki.tex \
			results/kl/table3.dict-kl.errm-ed3-no-initial.corp-wiki.tex

results/table3.tex: $(TABLE3_DATA)
	#echo "Reference impl. &" > $@
	#paste -d '&' results/en/table1.dict-hunspell.errm-hunspell.corp-wiki.tex \
	#	results/fi/table1.dict-voikko.errm-voikko.corp-wiki.tex >> $@
	#echo "& ---  \\\\\\\\" >> $@
	#echo "Generate all edits 2 & " >> $@
	#paste -d '&' results/en/table3.dict-norvig.errm-ed2.corp-wiki.generate-all.tex \
	#	results/fi/table3.dict-omorfi.errm-ed2.corp-wiki.generate-all.tex \
	#	results/kl/table3.dict-kl.errm-ed2.corp-wiki.generate-all.tex >> $@
	#echo "\\\\\\\\" >> $@
	echo "\\hline" >> $@
	echo "Edit distance 1 &" >> $@
	paste -d '&' results/en/table3.dict-norvig.errm-ed1.corp-wiki.tex \
		results/fi/table3.dict-omorfi.errm-ed1.corp-wiki.tex \
		results/kl/table3.dict-kl.errm-ed1.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "Edit distance 2 &" >> $@
	paste -d '&' results/en/table3.dict-norvig.errm-ed2.corp-wiki.tex \
		results/fi/table3.dict-omorfi.errm-ed2.corp-wiki.tex \
		results/kl/table3.dict-kl.errm-ed2.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "Edit distance 3 &" >> $@
	paste -d '&' results/en/table3.dict-norvig.errm-ed3.corp-wiki.tex \
		results/fi/table3.dict-omorfi.errm-ed3.corp-wiki.tex \
		results/kl/table3.dict-kl.errm-ed3.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "No first edits ed 1 & " >> $@
	paste -d '&' results/en/table3.dict-norvig.errm-ed1-no-initial.corp-wiki.tex \
		results/fi/table3.dict-omorfi.errm-ed1-no-initial.corp-wiki.tex \
		results/kl/table3.dict-kl.errm-ed1-no-initial.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "No first edits ed 2 &" >> $@
	paste -d '&' results/en/table3.dict-norvig.errm-ed2-no-initial.corp-wiki.tex \
		results/fi/table3.dict-omorfi.errm-ed2-no-initial.corp-wiki.tex \
		results/kl/table3.dict-kl.errm-ed2-no-initial.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "No first edits ed 3 &" >> $@
	paste -d '&' results/en/table3.dict-norvig.errm-ed3-no-initial.corp-wiki.tex \
		results/fi/table3.dict-omorfi.errm-ed3-no-initial.corp-wiki.tex \
		results/kl/table3.dict-kl.errm-ed3-no-initial.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "Avoid redundancy and first edits ed 2 &" >> $@
	#paste -d '&' results/en/table3.dict-norvig.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
	#	results/fi/table3.dict-omorfi.errm-ed2-no-initial-no-redundant.corp-wiki.tex \
	#	results/kl/table3.dict-kl.errm-ed2-no-initial-no-redundant.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "Avoid redundancy and first edits ed 3 &" >> $@
	#paste -d '&' results/en/table3.dict-norvig.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
	#	results/fi/table3.dict-omorfi.errm-ed3-no-initial-no-redundant.corp-wiki.tex \
	#	results/kl/table3.dict-kl.errm-ed3-no-initial-no-redundant.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "Lower order first ed 1 to 2 &" >> $@
	#paste -d '&' results/en/table3.dict-norvig.errm-fallback.corp-wiki.tex \
	#	results/fi/table3.dict-omorfi.errm-fallback.corp-wiki.tex \
	#	results/kl/table3.dict-kl.errm-fallback.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@



# Table 3, ED 1

results/en/table3.dict-norvig.errm-ed1.corp-wiki.tex: corp/en/automisp.words
	cut -f 1 corp/en/automisp.words | hfst-ospell-fsmnlp-2012 hfst/en/errmodel.edit-distance-1.hfst hfst/en/norvig.hfst > corp/en/temp_mispell_corrections
	cut -f 2 corp/en/automisp.words | hfst-optimized-lookup hfst/en/norvig.hfst > corp/en/temp_correct_corrections
	cut -f 2 corp/en/automisp.words > corp/en/temp_original_correct_words
	python tally_corrections.py corp/en/temp_mispell_corrections corp/en/temp_correct_corrections corp/en/temp_original_correct_words > $@

results/fi/table3.dict-omorfi.errm-ed1.corp-wiki.tex: corp/fi/automisp.words
	cut -f 1 corp/fi/automisp.words | hfst-ospell-fsmnlp-2012 hfst/fi/errmodel.edit-distance-1.hfst $(FI_DICT) > corp/fi/temp_mispell_corrections
	cut -f 2 corp/fi/automisp.words | hfst-optimized-lookup $(FI_DICT) > corp/fi/temp_correct_corrections
	cut -f 2 corp/fi/automisp.words > corp/fi/temp_original_correct_words
	python tally_corrections.py corp/fi/temp_mispell_corrections corp/fi/temp_correct_corrections corp/fi/temp_original_correct_words > $@

results/kl/table3.dict-kl.errm-ed1.corp-wiki.tex: corp/kl/automisp.words
	cut -f 1 corp/kl/automisp.words | hfst-ospell-fsmnlp-2012 hfst/kl/errmodel.edit-distance-1.hfst $(KL_DICT) > corp/kl/temp_mispell_corrections
	cut -f 2 corp/kl/automisp.words | hfst-optimized-lookup $(KL_DICT) > corp/kl/temp_correct_corrections
	cut -f 2 corp/kl/automisp.words > corp/kl/temp_original_correct_words
	python tally_corrections.py corp/kl/temp_mispell_corrections corp/kl/temp_correct_corrections corp/kl/temp_original_correct_words > $@

# Table 3, ED 2

results/en/table3.dict-norvig.errm-ed2.corp-wiki.tex: corp/en/automisp.words
	cut -f 1 corp/en/automisp.words | hfst-ospell-fsmnlp-2012 hfst/en/errmodel.edit-distance-2.hfst hfst/en/norvig.hfst > corp/en/temp_mispell_corrections
	cut -f 2 corp/en/automisp.words | hfst-optimized-lookup hfst/en/norvig.hfst > corp/en/temp_correct_corrections
	cut -f 2 corp/en/automisp.words > corp/en/temp_original_correct_words
	python tally_corrections.py corp/en/temp_mispell_corrections corp/en/temp_correct_corrections corp/en/temp_original_correct_words > $@

results/fi/table3.dict-omorfi.errm-ed2.corp-wiki.tex: corp/fi/automisp.words
	cut -f 1 corp/fi/automisp.words | hfst-ospell-fsmnlp-2012 hfst/fi/errmodel.edit-distance-2.hfst $(FI_DICT) > corp/fi/temp_mispell_corrections
	cut -f 2 corp/fi/automisp.words | hfst-optimized-lookup $(FI_DICT) > corp/fi/temp_correct_corrections
	cut -f 2 corp/fi/automisp.words > corp/fi/temp_original_correct_words
	python tally_corrections.py corp/fi/temp_mispell_corrections corp/fi/temp_correct_corrections corp/fi/temp_original_correct_words > $@

results/kl/table3.dict-kl.errm-ed2.corp-wiki.tex: corp/kl/automisp.words
	cut -f 1 corp/kl/automisp.words | hfst-ospell-fsmnlp-2012 hfst/kl/errmodel.edit-distance-2.hfst $(KL_DICT) > corp/kl/temp_mispell_corrections
	cut -f 2 corp/kl/automisp.words | hfst-optimized-lookup $(KL_DICT) > corp/kl/temp_correct_corrections
	cut -f 2 corp/kl/automisp.words > corp/kl/temp_original_correct_words
	python tally_corrections.py corp/kl/temp_mispell_corrections corp/kl/temp_correct_corrections corp/kl/temp_original_correct_words > $@

# Table 3, ED 3

results/en/table3.dict-norvig.errm-ed3.corp-wiki.tex: corp/en/automisp.words
	cut -f 1 corp/en/automisp.words | hfst-ospell-fsmnlp-2012 hfst/en/errmodel.edit-distance-3.hfst hfst/en/norvig.hfst > corp/en/temp_mispell_corrections
	cut -f 2 corp/en/automisp.words | hfst-optimized-lookup hfst/en/norvig.hfst > corp/en/temp_correct_corrections
	cut -f 2 corp/en/automisp.words > corp/en/temp_original_correct_words
	python tally_corrections.py corp/en/temp_mispell_corrections corp/en/temp_correct_corrections corp/en/temp_original_correct_words > $@

results/fi/table3.dict-omorfi.errm-ed3.corp-wiki.tex: corp/fi/automisp.words
	cut -f 1 corp/fi/automisp.words | hfst-ospell-fsmnlp-2012 hfst/fi/errmodel.edit-distance-3.hfst $(FI_DICT) > corp/fi/temp_mispell_corrections
	cut -f 2 corp/fi/automisp.words | hfst-optimized-lookup $(FI_DICT) > corp/fi/temp_correct_corrections
	cut -f 2 corp/fi/automisp.words > corp/fi/temp_original_correct_words
	python tally_corrections.py corp/fi/temp_mispell_corrections corp/fi/temp_correct_corrections corp/fi/temp_original_correct_words > $@

results/kl/table3.dict-kl.errm-ed3.corp-wiki.tex: corp/kl/automisp.words
	cut -f 1 corp/kl/automisp.words | hfst-ospell-fsmnlp-2012 hfst/kl/errmodel.edit-distance-3.hfst $(KL_DICT) > corp/kl/temp_mispell_corrections
	cut -f 2 corp/kl/automisp.words | hfst-optimized-lookup $(KL_DICT) > corp/kl/temp_correct_corrections
	cut -f 2 corp/kl/automisp.words > corp/kl/temp_original_correct_words
	python tally_corrections.py corp/kl/temp_mispell_corrections corp/kl/temp_correct_corrections corp/kl/temp_original_correct_words > $@

# Table 3, ED 1, no initial

results/en/table3.dict-norvig.errm-ed1-no-initial.corp-wiki.tex: corp/en/automisp.words
	cut -f 1 corp/en/automisp.words | hfst-ospell-fsmnlp-2012 hfst/en/errmodel.edit-distance-1-no-initial.hfst hfst/en/norvig.hfst > corp/en/temp_mispell_corrections
	cut -f 2 corp/en/automisp.words | hfst-optimized-lookup hfst/en/norvig.hfst > corp/en/temp_correct_corrections
	cut -f 2 corp/en/automisp.words > corp/en/temp_original_correct_words
	python tally_corrections.py corp/en/temp_mispell_corrections corp/en/temp_correct_corrections corp/en/temp_original_correct_words > $@

results/fi/table3.dict-omorfi.errm-ed1-no-initial.corp-wiki.tex: corp/fi/automisp.words
	cut -f 1 corp/fi/automisp.words | hfst-ospell-fsmnlp-2012 hfst/fi/errmodel.edit-distance-1-no-initial.hfst $(FI_DICT) > corp/fi/temp_mispell_corrections
	cut -f 2 corp/fi/automisp.words | hfst-optimized-lookup $(FI_DICT) > corp/fi/temp_correct_corrections
	cut -f 2 corp/fi/automisp.words > corp/fi/temp_original_correct_words
	python tally_corrections.py corp/fi/temp_mispell_corrections corp/fi/temp_correct_corrections corp/fi/temp_original_correct_words > $@

results/kl/table3.dict-kl.errm-ed1-no-initial.corp-wiki.tex: corp/kl/automisp.words
	cut -f 1 corp/kl/automisp.words | hfst-ospell-fsmnlp-2012 hfst/kl/errmodel.edit-distance-1-no-initial.hfst $(KL_DICT) > corp/kl/temp_mispell_corrections
	cut -f 2 corp/kl/automisp.words | hfst-optimized-lookup $(KL_DICT) > corp/kl/temp_correct_corrections
	cut -f 2 corp/kl/automisp.words > corp/kl/temp_original_correct_words
	python tally_corrections.py corp/kl/temp_mispell_corrections corp/kl/temp_correct_corrections corp/kl/temp_original_correct_words > $@

# Table 3, ED 2, no initial

results/en/table3.dict-norvig.errm-ed2-no-initial.corp-wiki.tex: corp/en/automisp.words
	cut -f 1 corp/en/automisp.words | hfst-ospell-fsmnlp-2012 hfst/en/errmodel.edit-distance-2-no-initial.hfst hfst/en/norvig.hfst > corp/en/temp_mispell_corrections
	cut -f 2 corp/en/automisp.words | hfst-optimized-lookup hfst/en/norvig.hfst > corp/en/temp_correct_corrections
	cut -f 2 corp/en/automisp.words > corp/en/temp_original_correct_words
	python tally_corrections.py corp/en/temp_mispell_corrections corp/en/temp_correct_corrections corp/en/temp_original_correct_words > $@

results/fi/table3.dict-omorfi.errm-ed2-no-initial.corp-wiki.tex: corp/fi/automisp.words
	cut -f 1 corp/fi/automisp.words | hfst-ospell-fsmnlp-2012 hfst/fi/errmodel.edit-distance-2-no-initial.hfst $(FI_DICT) > corp/fi/temp_mispell_corrections
	cut -f 2 corp/fi/automisp.words | hfst-optimized-lookup $(FI_DICT) > corp/fi/temp_correct_corrections
	cut -f 2 corp/fi/automisp.words > corp/fi/temp_original_correct_words
	python tally_corrections.py corp/fi/temp_mispell_corrections corp/fi/temp_correct_corrections corp/fi/temp_original_correct_words > $@

results/kl/table3.dict-kl.errm-ed2-no-initial.corp-wiki.tex: corp/kl/automisp.words
	cut -f 1 corp/kl/automisp.words | hfst-ospell-fsmnlp-2012 hfst/kl/errmodel.edit-distance-2-no-initial.hfst $(KL_DICT) > corp/kl/temp_mispell_corrections
	cut -f 2 corp/kl/automisp.words | hfst-optimized-lookup $(KL_DICT) > corp/kl/temp_correct_corrections
	cut -f 2 corp/kl/automisp.words > corp/kl/temp_original_correct_words
	python tally_corrections.py corp/kl/temp_mispell_corrections corp/kl/temp_correct_corrections corp/kl/temp_original_correct_words > $@

# Table 3, ED 3, no initial

results/en/table3.dict-norvig.errm-ed3-no-initial.corp-wiki.tex: corp/en/automisp.words
	cut -f 1 corp/en/automisp.words | hfst-ospell-fsmnlp-2012 hfst/en/errmodel.edit-distance-3-no-initial.hfst hfst/en/norvig.hfst > corp/en/temp_mispell_corrections
	cut -f 2 corp/en/automisp.words | hfst-optimized-lookup hfst/en/norvig.hfst > corp/en/temp_correct_corrections
	cut -f 2 corp/en/automisp.words > corp/en/temp_original_correct_words
	python tally_corrections.py corp/en/temp_mispell_corrections corp/en/temp_correct_corrections corp/en/temp_original_correct_words > $@

results/fi/table3.dict-omorfi.errm-ed3-no-initial.corp-wiki.tex: corp/fi/automisp.words
	cut -f 1 corp/fi/automisp.words | hfst-ospell-fsmnlp-2012 hfst/fi/errmodel.edit-distance-3-no-initial.hfst $(FI_DICT) > corp/fi/temp_mispell_corrections
	cut -f 2 corp/fi/automisp.words | hfst-optimized-lookup $(FI_DICT) > corp/fi/temp_correct_corrections
	cut -f 2 corp/fi/automisp.words > corp/fi/temp_original_correct_words
	python tally_corrections.py corp/fi/temp_mispell_corrections corp/fi/temp_correct_corrections corp/fi/temp_original_correct_words > $@

results/kl/table3.dict-kl.errm-ed3-no-initial.corp-wiki.tex: corp/kl/automisp.words
	cut -f 1 corp/kl/automisp.words | hfst-ospell-fsmnlp-2012 hfst/kl/errmodel.edit-distance-3-no-initial.hfst $(KL_DICT) > corp/kl/temp_mispell_corrections
	cut -f 2 corp/kl/automisp.words | hfst-optimized-lookup $(KL_DICT) > corp/kl/temp_correct_corrections
	cut -f 2 corp/kl/automisp.words > corp/kl/temp_original_correct_words
	python tally_corrections.py corp/kl/temp_mispell_corrections corp/kl/temp_correct_corrections corp/kl/temp_original_correct_words > $@

# Table 4 (text type vs. speed)

TABLE4_DATA=\
			results/en/table4.dict-norvig.errm-ed2-no-redundant.corp-wiki-automispelled.tex\
			results/fi/table4.dict-omorfi.errm-ed2-no-redundant.corp-wiki-automispelled.tex\
			results/kl/table4.dict-kl.errm-ed2-no-redundant.corp-wiki-automispelled.tex \
			results/en/table4.dict-norvig.errm-ed2-no-redundant.corp-wiki-wronglang.tex\
			results/fi/table4.dict-omorfi.errm-ed2-no-redundant.corp-wiki-wronglang.tex\
			results/kl/table4.dict-kl.errm-ed2-no-redundant.corp-wiki-wronglang.tex

results/table4.tex: $(TABLE1_DATA) $(TABLE4_DATA)
	# this baseline is from test table 1
	echo "Native Lang. Corpus &" > $@
	paste -d '&' results/en/table1.dict-norvig.errm-ed2-no-redundant.corp-wiki.tex \
		results/fi/table1.dict-omorfi.errm-ed2-no-redundant.corp-wiki.tex \
		results/kl/table1.dict-kl.errm-ed2-no-redundant.corp-wiki.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "Added automatic errors &" >> $@
	paste -d '&' results/en/table4.dict-norvig.errm-ed2-no-redundant.corp-wiki-automispelled.tex \
		results/fi/table4.dict-omorfi.errm-ed2-no-redundant.corp-wiki-automispelled.tex \
		results/kl/table4.dict-kl.errm-ed2-no-redundant.corp-wiki-automispelled.tex >> $@
	echo "\\\\\\\\" >> $@
	echo "Text in another language &" >> $@
	paste -d '&' results/en/table4.dict-norvig.errm-ed2-no-redundant.corp-wiki-wronglang.tex \
		results/fi/table4.dict-omorfi.errm-ed2-no-redundant.corp-wiki-wronglang.tex \
		results/kl/table4.dict-kl.errm-ed2-no-redundant.corp-wiki-wronglang.tex >> $@
	echo "\\\\\\\\" >> $@

# Automispelled

results/en/table4.dict-norvig.errm-ed2-no-redundant.corp-wiki-automispelled.tex: corp/en/automisp.words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2.hfst hfst/en/norvig.hfst < corp/en/automisp.words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table4.dict-omorfi.errm-ed2-no-redundant.corp-wiki-automispelled.tex: corp/fi/automisp.words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2-no-redundant.hfst $(FI_DICT) < corp/fi/automisp.words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table4.dict-kl.errm-ed2-no-redundant.corp-wiki-automispelled.tex: corp/kl/automisp.words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2-no-redundant.hfst $(KL_DICT) < corp/kl/automisp.words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# Wrong language

results/en/table4.dict-norvig.errm-ed2-no-redundant.corp-wiki-wronglang.tex: corp/fi/fiwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-2.hfst hfst/en/norvig.hfst < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/fi/table4.dict-omorfi.errm-ed2-no-redundant.corp-wiki-wronglang.tex: corp/kl/klwiki.$(WORDS).words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/fi/errmodel.edit-distance-2-no-redundant.hfst $(FI_DICT) < corp/kl/klwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/kl/table4.dict-kl.errm-ed2-no-redundant.corp-wiki-wronglang.tex: corp/fi/automisp.words
	echo > $@.tsv
	for i in $(TESTS) ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/kl/errmodel.edit-distance-2-no-redundant.hfst $(KL_DICT) < corp/fi/fiwiki.$(WORDS).words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

# table 5 and such
TABLE5_DATA=\
			results/en/table5.norvig.sigmasetsize \
			results/fi/table5.omorfi-20110505.sigmasetsize \
			results/kl/table5.kl.sigmasetsize  \
			results/en/table5.errmodel.edit-distance-1.sizes \
			results/en/table5.errmodel.edit-distance-1-no-initial.sizes \
			results/en/table5.errmodel.edit-distance-2.sizes \
			results/en/table5.errmodel.edit-distance-2-no-initial.sizes \
			results/en/table5.errmodel.edit-distance-2-no-initial-no-redundant.sizes \
			results/en/table5.errmodel.edit-distance-2-no-redundant.sizes \
			results/en/table5.errmodel.edit-distance-3.sizes \
			results/en/table5.errmodel.edit-distance-3-no-initial.sizes \
			results/en/table5.errmodel.edit-distance-3-no-initial-no-redundant.sizes \
			results/en/table5.errmodel.edit-distance-3-no-redundant.sizes \
			results/fi/table5.errmodel.edit-distance-1.sizes \
			results/fi/table5.errmodel.edit-distance-1-no-initial.sizes \
			results/fi/table5.errmodel.edit-distance-2.sizes \
			results/fi/table5.errmodel.edit-distance-2-no-initial.sizes \
			results/fi/table5.errmodel.edit-distance-2-no-initial-no-redundant.sizes \
			results/fi/table5.errmodel.edit-distance-2-no-redundant.sizes \
			results/fi/table5.errmodel.edit-distance-3.sizes \
			results/fi/table5.errmodel.edit-distance-3-no-initial.sizes \
			results/fi/table5.errmodel.edit-distance-3-no-initial-no-redundant.sizes \
			results/fi/table5.errmodel.edit-distance-3-no-redundant.sizes \
			results/kl/table5.errmodel.edit-distance-1.sizes \
			results/kl/table5.errmodel.edit-distance-1-no-initial.sizes \
			results/kl/table5.errmodel.edit-distance-2.sizes \
			results/kl/table5.errmodel.edit-distance-2-no-initial.sizes \
			results/kl/table5.errmodel.edit-distance-2-no-initial-no-redundant.sizes \
			results/kl/table5.errmodel.edit-distance-2-no-redundant.sizes \
			results/kl/table5.errmodel.edit-distance-3.sizes \
			results/kl/table5.errmodel.edit-distance-3-no-initial.sizes \
			results/kl/table5.errmodel.edit-distance-3-no-initial-no-redundant.sizes \
			results/kl/table5.errmodel.edit-distance-3-no-redundant.sizes

results/table5.tex: $(TABLE5_DATA)
	echo "Sigma set size &" > $@
	paste -d '&' results/en/table5.norvig.sigmasetsize \
		results/fi/table5.omorfi-20110505.sigmasetsize \
		results/kl/table5.kl.sigmasetsize >> $@
	echo "\\\\\\\\" >> $@
	echo "Edit distance 1 &" >> $@
	paste -d '&' results/en/table5.errmodel.edit-distance-1.sizes \
		results/fi/table5.errmodel.edit-distance-1.sizes \
		results/kl/table5.errmodel.edit-distance-1.sizes >> $@
	echo "\\\\\\\\" >> $@
	echo "Edit distance 2 &" >> $@
	paste -d '&' results/en/table5.errmodel.edit-distance-2.sizes \
		results/fi/table5.errmodel.edit-distance-2.sizes \
		results/kl/table5.errmodel.edit-distance-2.sizes >> $@
	echo "\\\\\\\\" >> $@
	echo "Edit distance 3 &" >> $@
	paste -d '&' results/en/table5.errmodel.edit-distance-3.sizes \
		results/fi/table5.errmodel.edit-distance-3.sizes \
		results/kl/table5.errmodel.edit-distance-3.sizes >> $@
	echo "\\\\\\\\" >> $@
	echo "No first edits ed 1 & " >> $@
	paste -d '&' results/en/table5.errmodel.edit-distance-1-no-initial.sizes \
		results/fi/table5.errmodel.edit-distance-1-no-initial.sizes \
		results/kl/table5.errmodel.edit-distance-1-no-initial.sizes >> $@
	echo "\\\\\\\\" >> $@
	echo "No first edits ed 2 &" >> $@
	paste -d '&' results/en/table5.errmodel.edit-distance-2-no-initial.sizes \
		results/fi/table5.errmodel.edit-distance-2-no-initial.sizes \
		results/kl/table5.errmodel.edit-distance-2-no-initial.sizes >> $@
	echo "\\\\\\\\" >> $@
	echo "No first edits ed 3 &" >> $@
	paste -d '&' results/en/table5.errmodel.edit-distance-3-no-initial.sizes \
		results/fi/table5.errmodel.edit-distance-3-no-initial.sizes \
		results/kl/table5.errmodel.edit-distance-3-no-initial.sizes >> $@
	echo "\\\\\\\\" >> $@
	echo "Avoid redundancy and first edits ed 2 &" >> $@
	paste -d '&' results/en/table5.errmodel.edit-distance-2-no-initial-no-redundant.sizes \
		results/fi/table5.errmodel.edit-distance-2-no-initial-no-redundant.sizes \
		results/kl/table5.errmodel.edit-distance-2-no-initial-no-redundant.sizes >> $@
	echo "\\\\\\\\" >> $@
	echo "Avoid redundancy and first edits ed 3 &" >> $@
	paste -d '&' results/en/table5.errmodel.edit-distance-3-no-initial-no-redundant.sizes \
		results/fi/table5.errmodel.edit-distance-3-no-initial-no-redundant.sizes \
		results/kl/table5.errmodel.edit-distance-3-no-initial-no-redundant.sizes >> $@
	echo "\\\\\\\\" >> $@
	echo "Lower order first ed 1 to 2 &" >> $@
	echo "&&" >> $@
	echo "\\\\\\\\" >> $@

results/en/table5.%.sigmasetsize: hfst/en/%.hfst
	hfst-summarize -v $< |\
		fgrep 'sigma set size' |\
		sed -e 's/^.*://' > $@

results/fi/table5.%.sigmasetsize: hfst/fi/%.hfst
	hfst-summarize -v $< |\
		fgrep 'sigma set size' |\
		sed -e 's/^.*://' > $@

results/kl/table5.%.sigmasetsize: hfst/kl/%.hfst
	hfst-summarize -v $< |\
		fgrep 'sigma set size' |\
		sed -e 's/^.*://' > $@

results/en/table5.%.sizes: hfst/en/%.hfst
	hfst-summarize $< |\
		grep '# of \(states\|arcs\)' |\
		sed -e 's/^.*://' |\
		tr -s "\n" "," > $@

results/fi/table5.%.sizes: hfst/fi/%.hfst
	hfst-summarize $< |\
		grep '# of \(states\|arcs\)' |\
		sed -e 's/^.*://' |\
		tr -s "\n" "," > $@

results/kl/table5.%.sizes: hfst/kl/%.hfst
	hfst-summarize $< |\
		grep '# of \(states\|arcs\)' |\
		sed -e 's/^.*://' |\
		tr -s "\n" "," > $@


