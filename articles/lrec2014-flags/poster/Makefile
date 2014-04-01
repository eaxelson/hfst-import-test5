# Execute this Makefile with GNU compatible make to recreate the test results
# and paper for article titled
# # `blah' in Proceedings of EACL 2014

XELATEX=xelatex
PDFLATEX=pdflatex

.PHONY: articles clean

# article
articles: lrec2014-poster.pdf

%.pdf: %.dot
	dot -Tpdf -o$@ $<

lrec2014-poster.pdf: lrec2014-poster.tex
	$(PDFLATEX) lrec2014-poster.tex
	$(PDFLATEX) lrec2014-poster.tex

clean:
	-rm -f *.aux *.log *.blg *.pdf *.nav *.toc *.out *.snm *.vrb
