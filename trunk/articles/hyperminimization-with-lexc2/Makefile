# Execute this Makefile with GNU compatible make to recreate the test results
# and paper for article titled
# # `blah' in Proceedings of EACL 2014

XELATEX=xelatex
BIBTEX=bibtex

.PHONY: articles clean

# article
articles: hyperminimization-with-lexc2.pdf

%.pdf: %.dot
	dot -Tpdf -o$@ $<

hyperminimization-with-lexc2.pdf: hyperminimization-with-lexc2.tex hyperminimization-with-lexc2.bib
	$(XELATEX) hyperminimization-with-lexc2.tex
	$(BIBTEX)  hyperminimization-with-lexc2
	$(XELATEX) hyperminimization-with-lexc2.tex
	$(XELATEX) hyperminimization-with-lexc2.tex
	$(XELATEX) hyperminimization-with-lexc2.tex
	
clean:
	-rm -f hyperminimization-with-lexc2.aux hyperminimization-with-lexc2.bbl hyperminimization-with-lexc2.log hyperminimization-with-lexc2.blg hyperminimization-with-lexc2.pdf
