all:Using\ HFST\ for\ Creating\ Computational\ Linguistic\ Applications.pdf

Using\ HFST\ for\ Creating\ Computational\ Linguistic\ Applications.pdf:Using\ HFST\ for\ Creating\ Computational\ Linguistic\ Applications.tex hfst2012.bib
	pdflatex Using\ HFST\ for\ Creating\ Computational\ Linguistic\ Applications
	bibtex   Using\ HFST\ for\ Creating\ Computational\ Linguistic\ Applications
	pdflatex Using\ HFST\ for\ Creating\ Computational\ Linguistic\ Applications
	pdflatex Using\ HFST\ for\ Creating\ Computational\ Linguistic\ Applications
	pdflatex Using\ HFST\ for\ Creating\ Computational\ Linguistic\ Applications


## Use make to create article pdfs or recreate test results

TEX=pdflatex
BIB=bibtex
SCP=scp
SCPHOST=ruuvi
SCPDIR=public_html/tmp/
PAPER_SOURCES=cla-2012-hfst.tex hfst2012.bib
PAPER_PDF=cla-2012-hfst.pdf


cla-2012-hfst.pdf: cla-2012-hfst.tex hfst2012.bib
	$(TEX) cla-2012-hfst.tex
	$(BIB) cla-2012-hfst
	$(TEX) cla-2012-hfst.tex
	$(TEX) cla-2012-hfst.tex
	$(TEX) cla-2012-hfst.tex

