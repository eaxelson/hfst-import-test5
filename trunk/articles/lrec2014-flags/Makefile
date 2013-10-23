# Execute this Makefile with GNU compatible make to recreate the test results
# and paper for article titled
# # `blah' in Proceedings of EACL 2014

XELATEX=xelatex
BIBTEX=bibtex

.PHONY: articles clean

# article
articles: lrec2014-abstract.pdf eacl2014-hyperminimization.pdf

%.pdf: %.dot
	dot -Tpdf -o$@ $<

lrec2014-abstract.pdf: lrec2014-abstract.tex lrec2014.bib
	$(XELATEX) lrec2014-abstract.tex
	$(BIBTEX)  lrec2014-abstract
	$(XELATEX) lrec2014-abstract.tex
	$(XELATEX) lrec2014-abstract.tex
	$(XELATEX) lrec2014-abstract.tex
	
clean:
	-rm -f lrec2014-abstract.aux lrec2014-abstract.bbl lrec2014-abstract.log lrec2014-abstract.blg
