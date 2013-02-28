PDFLATEX = xelatex
LATEXFLAGS	= 
BIBTEX		= bibtex
BIBTEXFLAGS	= 
SCP			= scp
SCPFLAGS	= 
REMOTE_DIR	= tapirine@ruuvi.helsinki.fi:public_html/paperit/

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

