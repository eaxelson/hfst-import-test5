TEX=xelatex
BIB=bibtex
OMORFI=omorfi/fi

prefix=/usr/local
sharedir=$(prefix)/share

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

$(OMORFI)/error.edit-distance-2-maxcw.hfst: $(sharedir)/hfst/fi/error.edit-distance-2.hfst
	cp -v $< $@

results/fiwiki.dictionary-full-unweighted.edit-distance-2-maxcw.results: fi-FI.fiwiki-gutenberg $(OMORFI)/dictionary.default.hfst $(OMORFI)/error.edit-distance-2-maxcw.hfst

cicling-2011-contextspell.pdf: cicling-2011-contextspell.tex cicling2011.bib
	$(TEX) cicling-2011-contextspell.tex
	$(BIB) cicling-2011-contextspell
	$(TEX) cicling-2011-contextspell.tex
	$(TEX) cicling-2011-contextspell.tex
	$(TEX) cicling-2011-contextspell.tex

clean:
	-rm -f cicling-2011-contextspell.aux cicling-2011-contextspell.bbl cicling-2011-contextspell.pdf
