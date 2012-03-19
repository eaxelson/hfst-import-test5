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

# article
fsmnlp-2012-speedspelling.pdf: fsmnlp-2012-speedspelling.tex fsmnlp2012.bib
	$(XELATEX) fsmnlp-2012-speedspelling.tex
	$(BIBTEX)  fsmnlp-2012-speedspelling
	$(XELATEX) fsmnlp-2012-speedspelling.tex
	$(XELATEX) fsmnlp-2012-speedspelling.tex
	$(XELATEX) fsmnlp-2012-speedspelling.tex

results: corpora dictionaries models tables
corpora:	corp/fi/fiwiki-20120213-pages-articles.10000000.xml \
		corp/en/enwiki-20120211-pages-articles.10000000.xml \
		corp/kl/klwiki-20120214-pages-articles.xml \
		corp/en/enwiki.words \
		corp/fi/fiwiki.words \
		corp/kl/klwiki.words \
		corp/en/automisp.words \
		corp/fi/automisp.words \
		corp/kl/automisp.words
dictionaries:	hfst/en/norvig.hfst \
		hfst/fi/omorfi-20110505.hfst \
		hfst/kl/kl.hfst
models:		hfst/fi/errmodel.edit-distance-1.hfst \
		hfst/en/errmodel.edit-distance-1.hfst \
		hfst/kl/errmodel.edit-distance-1.hfst
tables:		results/table1.tex
clean-models:
	rm hfst/fi/errmodel.edit-distance-1.hfst \
	hfst/en/errmodel.edit-distance-1.hfst \
	hfst/kl/errmodel.edit-distance-1.hfst

# tests and tables...
#
# corpora acquisition
corp/fi/fiwiki-20120213-pages-articles.10000000.xml:
	$(WGET) 'http://dumps.wikimedia.org/fiwiki/20120213/fiwiki-20120213-pages-articles.xml.bz2'
	$(BZCAT) fiwiki-20120213-pages-articles.xml.bz2 | head -n 10000000 > $@

corp/en/enwiki-20120211-pages-articles.10000000.xml:
	$(WGET) 'http://dumps.wikimedia.org/enwiki/20120211/enwiki-20120211-pages-articles.xml.bz2'
	$(BZCAT) enwiki-20120211-pages-articles.xml.bz2 | head -n 10000000 > $@

corp/kl/klwiki-20120214-pages-articles.xml:
	$(WGET) 'http://dumps.wikimedia.org/klwiki/20120214/klwiki-20120214-pages-articles.xml.bz2'
	$(BZCAT) klwiki-20120214-pages-articles.xml.bz2 > $@

# corpora preprocessing (ugly fixme plz)
corp/en/enwiki.words: corp/en/enwiki-20120211-pages-articles.10000000.xml
	awk '/<text/,/<\/text/ {print}' < $< |\
		./wikipedia-deform.sh |\
		tr -s ' ' '\n' > $@

corp/fi/fiwiki.words: corp/fi/fiwiki-20120213-pages-articles.10000000.xml
	awk '/<text/,/<\/text/ {print}' < $< |\
		./wikipedia-deform.sh |\
		tr -s ' ' '\n' > $@

corp/kl/klwiki.words: corp/kl/klwiki-20120214-pages-articles.xml
	awk '/<text/,/<\/text/ {print}' < $< |\
		./wikipedia-deform.sh |\
		tr -s ' ' '\n' > $@

# to 1:1 recreate article results please be using --seed=15632753
corp/en/automisp.words: corp/en/enwiki.words
	python lines2misspell.py --input=$< --output=$@

corp/fi/automisp.words: corp/fi/fiwiki.words
	python lines2misspell.py --input=$< --output=$@

corp/kl/automisp.words: corp/kl/klwiki.words
	python lines2misspell.py --input=$< --output=$@

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
#
# Basic ED 1
hfst/fi/errmodel.edit-distance-1.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim -d 1 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-1.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim -d 1 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-1.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim -d 1 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@
# Basic ED 2
hfst/fi/errmodel.edit-distance-2.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-2.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-2.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@
# Basic ED 3
hfst/fi/errmodel.edit-distance-3.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-3.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-3.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 2, eliminate redundancies
hfst/fi/errmodel.edit-distance-2-no-redundant.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-2-no-redundant.hfst: hfst/en/norvig.hfst
	python editdist.py -s -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-2-no-redundant.hfst: hfst/kl/kl.hfst
	python editdist.py -s -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 3, eliminate redundancies
hfst/fi/errmodel.edit-distance-3-no-redundant.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s -d 3 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-3-no-redundant.hfst: hfst/en/norvig.hfst
	python editdist.py -s -d 3 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-3-no-redundant.hfst: hfst/kl/kl.hfst
	python editdist.py -s -d 3 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 1, no initial edit
hfst/fi/errmodel.edit-distance-1-no-initial.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 1 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-1-no-initial.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 1 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-1-no-initial.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 1 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 2, no initial edit
hfst/fi/errmodel.edit-distance-2-no-initial.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-2-no-initial.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-2-no-initial.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 3, no initial edit
hfst/fi/errmodel.edit-distance-3-no-initial.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 3 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-3-no-initial.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 3 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-3-no-initial.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-elim --no-string-initial-correction -d 3 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 2, no initial edit, eliminate redundancies
hfst/fi/errmodel.edit-distance-2-no-initial-no-redundant.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-string-initial-correction -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-2-no-initial-no-redundant.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-string-initial-correction -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-2-no-initial-no-redundant.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-string-initial-correction -d 2 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

# ED 3, no initial edit
hfst/fi/errmodel.edit-distance-3-no-initial-no-redundant.hfst: hfst/fi/omorfi-20110505.hfst
	python editdist.py -s --no-string-initial-correction -d 3 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/en/errmodel.edit-distance-3-no-initial-no-redundant.hfst: hfst/en/norvig.hfst
	python editdist.py -s --no-string-initial-correction -d 3 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@

hfst/kl/errmodel.edit-distance-3-no-initial-no-redundant.hfst: hfst/kl/kl.hfst
	python editdist.py -s --no-string-initial-correction -d 3 -a $< |\
		hfst-txt2fst -v |\
		hfst-minimize -v |\
		hfst-fst2fst -v -f olw -o $@


# (re)generate results in neat tables
# combine all bits and pieces for table 1:
results/table1.tex: results/en/table1.dict-norvig.errm-ed1.corp-wiki.tex #results/fi/table1.dict-omorfi.errm-ed1.corp-wiki.tex results/kl/table1.dict-kl.errm-ed1.corp-wiki.tex
	paste $^ > $@

results/en/table1.dict-norvig.errm-ed1.corp-wiki.tex: corp/en/enwiki.words
	echo > $@.tsv
	for i in 1 2 3 4 5 6 7 8 9 0 ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-1.hfst hfst/en/norvig.hfst < corp/en/enwiki.words > /dev/null ;\
	done
	awk -f profile-times2tex.awk < $@.tsv > $@

results/en/table1.dict-norvig.errm-ed2.corp-wiki.generate-all.tex: corp/en/enwiki.words
	echo > $@.time
	for i in 1 2 3 4 5 6 7 8 9 0 ; do \
		/usr/bin/time -a -o $@.time while read line; do \
			echo $line | hfst-optimized-lookup hfst/en/errormodel.edit-distance-2.hfst | cut -f 2 | hfst-optimized-lookup hfst/en/norvig.hfst > /dev/null ; done < corp/en/enwiki.words \
	done
# postprocessing step goes here


results/en/table2.dict-norvig.errm-ed1.corp-wiki.tex: corp/en/enwiki.words $@.tsv
	echo > $@.tsv
	for i in 1 2 3 4 5 6 7 8 9 0 ; do \
		hfst-ospell-fsmnlp-2012 --profile $@.tsv hfst/en/errmodel.edit-distance-1.hfst hfst/en/norvig.hfst < corp/en/enwiki.words > /dev/null ;\
	done
	awk -f profile-mems2tex.awk < $@.tsv > $@

