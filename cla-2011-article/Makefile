all:hfst-cla-2011.pdf

%.pdf:%.tex
	pdflatex $^
	bibtex $*
	pdflatex $^
	pdflatex $^

clean:
	-rm -f hfst-cla-2011.pdf hfst-cla-2011.aux hfst-cla-2011.bbl hfst-cla-2011.blg
