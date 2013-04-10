# Execute this Makefile with GNU compatible make to recreate the test results
# and paper for article titled
# # `blah' in Proceedings of FSMLNP 2013

XELATEX=xelatex
BIBTEX=bibtex

.PHONY: articles clean

# article
articles: fsmnlp-2013-flag-optimisation.pdf

%.pdf: %.dot
	dot -Tpdf -o$@ $<

fsmnlp-2013-flag-optimisation.pdf: fsmnlp-2013-flag-optimisation.tex fsmnlp2013flags.bib
	$(XELATEX) fsmnlp-2013-flag-optimisation.tex
	$(BIBTEX)  fsmnlp-2013-flag-optimisation
	$(XELATEX) fsmnlp-2013-flag-optimisation.tex
	$(XELATEX) fsmnlp-2013-flag-optimisation.tex
	$(XELATEX) fsmnlp-2013-flag-optimisation.tex

clean:
	-rm -f fsmnlp-2013-flag-optimisation.aux fsmnlp-2013-flag-optimisation.bbl \
		fsmnlp-2013-flag-optimisation.log fsmnlp-2013-flag-optimisation.blg
