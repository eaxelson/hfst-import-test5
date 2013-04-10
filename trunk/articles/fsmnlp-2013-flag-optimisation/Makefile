# Execute this Makefile with GNU compatible make to recreate the test results
# and paper for article titled
# # `blah' in Proceedings of FSMLNP 2013

XELATEX=xelatex

# article
articles: fsmnlp-2012-speedspelling.pdf fsmnlp-2012-spelling-tutorial.pdf

%.pdf: %.dot
	dot -Tpdf -o$@ $<

fsmnlp-2013-flag-optimisation.pdf: fsmnlp-2013-flag-optimisation.tex fsmnlp2013flags.bib
	$(XELATEX) fsmnlp-2012-flag-optimisation.tex
	$(BIBTEX)  fsmnlp-2012-flag-optimisation
	$(XELATEX) fsmnlp-2012-flag-optimisation.tex
	$(XELATEX) fsmnlp-2012-flag-optimisation.tex
	$(XELATEX) fsmnlp-2012-flag-optimisation.tex

