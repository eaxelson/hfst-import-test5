PDFLATEX = xelatex
LATEXFLAGS	= 
BIBTEX		= bibtex
BIBTEXFLAGS	= 
SCP			= scp
SCPFLAGS	= 

.PHONY: all

all: HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.pdf

# using pdflatex, xelatex etc.
HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.pdf: HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.tex sfcm-2013.bib
	$(PDFLATEX) $(PDFLATEXFLAGS) HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.tex
	if grep 'undefined citations' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null || \
			grep 'Citation.*undefined' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null ; then \
		$(BIBTEX) $(BIBTEXFLAGS) HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools ;\
		$(PDFLATEX) $(PDFLATEXFLAGS) HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.tex ;\
	fi
	if grep 'Warning: Label(s) may' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null || \
			grep 'Rerun' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null || \
			grep 'Warning: Citation' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null; then \
		$(PDFLATEX) $(PDFLATEXFLAGS) HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.tex ;\
	fi
	if grep 'Warning: Label(s) may' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null || \
			grep 'Rerun' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null || \
			grep 'Warning: Citation' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null; then \
		$(PDFLATEX) $(PDFLATEXFLAGS) HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.tex ;\
	fi
	if grep 'Warning: Label(s) may' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null || \
			grep 'Rerun' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null || \
			grep 'Warning: Citation' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null; then \
		$(PDFLATEX) $(PDFLATEXFLAGS) HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.tex ;\
	fi
	if grep 'Warning: Label(s) may' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null || \
			grep 'Rerun' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null || \
			grep 'Warning: Citation' HFST\ -\ a\ System\ for\ Creating\ NLP\ Tools.log > /dev/null; then \
		echo 'THE RESULT CONTAINS BROKEN REFERENCES, CITATIONS OR SOMETHING' ;\
	fi

# graphics for latex dvips
%.eps: %.png
	$(CONVERT) $< $(CONVERTFLAGS) $@

# {{{ speller examples from past articles
WGET=wget
BZCAT=bzcat
SED=sed
AWK=awk
WIKICLEAN=./wikipedia-deform.sh
HFST_COMPOSE=hfst-compose
HFST_CONCATENATE=hfst-concatenate
HFST_FST2FST=hfst-fst2fst
HFST_INVERT=hfst-invert
HFST_MINIMIZE=hfst-minimize
HFST_PROJECT=hfst-project
HFST_REGEXP2FST=hfst-regexp2fst
HFST_REPEAT=hfst-repeat
HFST_REWEIGHT=hfst-reweight
HFST_STRINGS2FST=hfst-strings2fst
HFST_TXT2FST=hfst-txt2fst
HFST_UNION=hfst-disjunct
HFST_PROC=hfst-proc
HFST_OSPELL=hfst-ospell-survey
HFST_LOOKUP=hfst-lookup
PYTHON2=python2
PYTHON3=python3
EDITDIST=editdist.py
ASPELL=aspell
HUNSPELL=hunspell
CONTEXT_SPELL1=hfst-ospell-cicling
CONTEXT_SPELL2=${HOME}/src/hfst-svn-trunk/articles/cicling-2011-contextspell/bin/context_speller
EN_TAGGER=${HOME}/src/hfst-svn-trunk/articles/cicling-2011-contextspell/taggers/en/hmm_model.lex ${HOME}/src/hfst-svn-trunk/articles/cicling-2011-contextspell/taggers/en/hmm_model.seq
# use GNU time rather than bash builtin
TIME=/usr/bin/time

# }}}
#
# {{{ language settings all
PROFILE_FOR_EXPR=1 2 3 4 5

TEST_CORPUS_HUGE=100000000
TEST_CORPUS_LARGE=1000000
TEST_CORPUS_SMALL=10000
TEST_CORPUS_TINY=100
CONTEXT_CORPUS_HUGE=100000000
CONTEXT_CORPUS_LARGE=1000000
CONTEXT_CORPUS_SMALL=10000
CONTEXT_CORPUS_TINY=100

# }}}

# {{{ German settings
DE_ALPHABET=abcdefghijklmnopqrstuvwxzyABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\'-

DE_TEST_CORPUS=corp/de/wikipedia-test.txt

DE_TEST_SORTS_HUGE=corp/de/wikipedia-test.$(TEST_CORPUS_HUGE).sorted
DE_TEST_SORTS_LARGE=corp/de/wikipedia-test.$(TEST_CORPUS_LARGE).sorted
DE_TEST_SORTS_SMALL=corp/de/wikipedia-test.$(TEST_CORPUS_SMALL).sorted
DE_TEST_SORTS_ONE=corp/de/wikipedia-test.1.sorted
DE_TEST_WORDS_HUGE=corp/de/wikipedia-test.$(TEST_CORPUS_HUGE).words
DE_TEST_WORDS_LARGE=corp/de/wikipedia-test.$(TEST_CORPUS_LARGE).words
DE_TEST_WORDS_SMALL=corp/de/wikipedia-test.$(TEST_CORPUS_SMALL).words
DE_TEST_WORDS_ONE=corp/de/wikipedia-test.1.words
DE_TEST_NONWORDS_LARGE=corp/de/wikipedia-test.$(TEST_CORPUS_LARGE).nonwords
DE_TEST_NONWORDS_SMALL=corp/de/wikipedia-test.$(TEST_CORPUS_SMALL).nonwords
DE_TEST_NONWORDS_ONE=corp/de/wikipedia-test.1.nonwords
DE_TEST_WORDS_REAL=corp/de/wikispells.tsv
DE_TEST_WORDS_HUGE=corp/de/wikipedia-test.$(TEST_CORPUS_HUGE).contexts
DE_CONTEXT_WORDS_LARGE=corp/de/wikipedia-test.$(TEST_CORPUS_LARGE).contexts
DE_CONTEXT_WORDS_SMALL=corp/de/wikipedia-test.$(TEST_CORPUS_SMALL).contexts
DE_CONTEXT_WORDS_ONE=corp/de/wikipedia-test.1.contexts
DE_CONTEXT_NONWORDS_LARGE=corp/de/wikipedia-test.$(TEST_CORPUS_LARGE).noncontexts
DE_CONTEXT_NONWORDS_SMALL=corp/de/wikipedia-test.$(TEST_CORPUS_SMALL).noncontexts
DE_CONTEXT_NONWORDS_ONE=corp/de/wikipedia-test.1.noncontexts

ERRM_DE=hfst/de/errmodel.ed2.hfst

DICT_DE=hfst/de/acceptor.morphisto.hfst
DEWIKI_DATE=20130219
CORP_DE_NORV=corp/de/big.txt
CORP_DE_WPLM=corp/de/wikipedia-train.txt

# }}}
#

DICTIONARIES=$(DICT_DE)

# coverage and quality tables

# {{{ German coverage and quality settings
RESULTS_EN_RUNNING=\
				   results/de/rung.spells.tsv
RESULTS_EN_REAL=\
				   results/de/real.spells.tsv

results-en: $(RESULTS_EN_RUNNING) $(RESULTS_DE_REAL)
# }}}
#
# {{{ German running text quality tests
results/de/rung.spells.tsv: $(DE_TEST_WORDS_LARGE) $(DICT_DE) $(ERRM_DE)
	$(HFST_OSPELL) -X results/de/rung.stats.tsv -H results/en/rung.hist.tsv $(ERRM_DE_5UNW) $(DICT_DE) < $(DE_TEST_WORDS_LARGE) > $@

results/de/real.spells.tsv: $(DE_TEST_WORDS_LARGE) $(DICT_DE) $(ERRM_DE)
	$(HFST_OSPELL) -X results/de/real.stats.tsv -H results/en/real.hist.tsv $(ERRM_DE_5UNW) $(DICT_DE) < $(DE_TEST_WORDS_LARGE) > $@

# }}}

# {{{ German profiling test path settings

PROFILES_DE=\
			 results/de/profiles.1-words.tsv \
			 results/de/profiles.$(TEST_CORPUS_SMALL)-words.tsv \
			 results/de/profiles.$(TEST_CORPUS_LARGE)-words.tsv


profiles: $(PROFILES_DE)

# }}}
#
# {{{ German profiling tests
results/de/profiles.1-words.tsv: $(DE_TEST_WORDS_ONE) $(DICT_DE) $(ERRM_DE)
	for i in $(PROFILE_FOR_EXPR); do \
		$(HFST_OSPELL) -P $@ $(ERRM_DE) $(DICT_DE) < $(DE_TEST_WORDS_ONE) > /dev/null ;\
	done

results/de/profiles.$(TEST_CORPUS_SMALL)-words.tsv: $(DE_TEST_WORDS_SMALL) $(DICT_DE) $(ERRM_DE)
	for i in $(PROFILE_FOR_EXPR); do \
		$(HFST_OSPELL) -P $@ $(ERRM_DE) $(DICT_DE) < $(DE_TEST_WORDS_SMALL) > /dev/null ;\
	done

results/de/profiles.$(TEST_CORPUS_LARGE)-words.tsv: $(DE_TEST_WORDS_LARGE) $(DICT_DE) $(ERRM_DE)
	for i in $(PROFILE_FOR_EXPR); do \
		$(HFST_OSPELL) -P $@ $(ERRM_DE) $(DICT_DE) < $(DE_TEST_WORDS_LARGE) > /dev/null ;\
	done


# {{{ German dictionary building
dictionaries: $(DICT_DE)

hfst/de/morphisto.a:
	$(WGET) http://morphisto.googlecode.com/files/morphisto-02022011.a -O $@

$(DICT_DE): hfst/de/morphisto.a
	$(HFST_PROJECT) -p output -v $< |\
		$(HFST_MINIMIZE) -v |\
		$(HFST_FST2FST) -v -f olw $< -o $@
# }}}
#
# {{{ German error model building
errmodels: $(ERRM_DE)

$(ERRM_DE): hfst/de/edit-2.hfst
	$(HFST_MINIMIZE) -v $< |\
		$(HFST_FST2FST) -v -f olw -o $@

hfst/de/%.hfst: manual/de/%.txt
	$(HFST_TXT2FST) -v $< -o $@

hfst/de/%.hfst: manual/de/%.strings
	$(HFST_STRINGS2FST) -j $< -o $@

# }}}

# corpora
# {{{ German corpora herding
corpora-de: corp/de/wikipedia-test.txt corp/de/wikipedia-train.txt corp/de/big.txt

corp/de/wikipedia-test.txt: corp/de/dewiki-pages-articles-test.xml
	$(AWK) '/<text/,/<\/text/ {print}' < $<|\
		$(SED) -e 's_</\?text[^>]*>__g' |\
		$(WIKICLEAN) |\
		iconv -c -f utf8 -t utf8//IGNORE > $@

corp/de/wikipedia-train.txt: corp/de/dewiki-pages-articles-train.xml
	$(AWK) '/<text/,/<\/text/ {print}' < $<|\
		$(SED) -e 's_</\?text[^>]*>__g' |\
		$(WIKICLEAN) |\
		iconv -c -f utf8 -t utf8//IGNORE > $@

corp/de/dewiki-pages-articles-$(DEWIKI_DATE).xml.bz2:
	$(WGET) http://dumps.wikimedia.org/dewiki/$(DEWIKI_DATE)/dewiki-$(DEWIKI_DATE)-pages-articles.xml.bz2 -O $@

corp/de/dewiki-pages-articles.lines: corp/de/dewiki-pages-articles-$(DEWIKI_DATE).xml.bz2
	$(BZCAT) $< | wc -l > $@

corp/de/dewiki-pages-articles-train.xml: corp/de/dewiki-pages-articles-$(DEWIKI_DATE).xml.bz2 corp/de/dewiki-pages-articles.lines
	$(BZCAT) $< | head -n `cat corp/de/dewiki-pages-articles.lines | sed -e 's/$$/ * 0.01/' | bc | sed -e 's/[.,].*//'` > $@

corp/de/dewiki-pages-articles-test.xml: corp/de/dewiki-pages-articles-$(DEWIKI_DATE).xml.bz2 corp/de/dewiki-pages-articles.lines
	$(BZCAT) $< | tail -n +`cat corp/de/dewiki-pages-articles.lines | sed -e 's/$$/ * 0.09/' | bc | sed -e 's/[.,].*//'` > $@


# }}}
#
# {{{ German corpora building
corp/de/%.sorted: corp/de/%.words
	sort < $< > $@

corp/de/%.$(TEST_CORPUS_HUGE).words: corp/de/%.txt
	tr -s '[:space:].?!,;:/' '\n' < $< |\
		grep "^[$(DE_ALPHABET)]*$$" |\
		grep '[[:alpha:]]' |\
		head -n $(TEST_CORPUS_HUGE) > $@

corp/de/%.$(TEST_CORPUS_LARGE).words: corp/de/%.txt
	tr -s '[:space:].?!,;:/' '\n' < $< |\
		grep "^[$(DE_ALPHABET)]*$$" |\
		grep '[[:alpha:]]' |\
		head -n $(TEST_CORPUS_LARGE) > $@

corp/de/%.$(TEST_CORPUS_SMALL).words: corp/de/%.txt
	tr -s '[:space:].?!,;:/' '\n' < $< |\
		grep "^[$(DE_ALPHABET)]*$$" |\
		grep '[[:alpha:]]' |\
		head -n $(TEST_CORPUS_SMALL) > $@

corp/de/%.1.words: corp/de/%.txt
	tr -s '[:space:].?!,;:/' '\n' < $< |\
		grep "^[$(DE_ALPHABET)]*$$" |\
		grep '[[:alpha:]]' |\
		head -n 1 > $@

corp/de/%.words: corp/de/%.txt
	tr -s '[:space:].?!,;:/' '\n' < $< |\
		grep "^[$(DE_ALPHABET)]*$$" |\
		grep '[[:alpha:]]' > $@


# }}}
#
#

# {{{ German error models
manual/de/edit-0.txt:
	$(PYTHON2) editdist.py -d 0 -s --no-elim "$(DE_ALPHABET)" |\
		fgrep -v "\\" > $@

manual/de/edit-1.txt:
	$(PYTHON2) editdist.py -s --no-elim "$(DE_ALPHABET)" |\
		fgrep -v "\\" > $@

manual/de/edit-2.txt:
	$(PYTHON2) editdist.py -s -d 2 --no-elim "$(DE_ALPHABET)" |\
		fgrep -v "\\" > $@


# foo

%.hfstol: %.hfst
	hfst-fst2fst -f olw $< -o $@


# vim: set foldmethod=marker:
