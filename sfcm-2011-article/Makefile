PDFLATEX = xelatex
LATEXFLAGS	= 
BIBTEX		= bibtex
BIBTEXFLAGS	= 
SCP			= scp
SCPFLAGS	= 
REMOTE_DIR	= tapirine@ruuvi.helsinki.fi:public_html/paperit/

# using pdflatex, xelatex etc.
%.pdf: %.tex
	$(PDFLATEX) $(PDFLATEXFLAGS) $<
	if grep 'undefined citations' $*.log > /dev/null || \
			grep 'Citation.*undefined' $*.log > /dev/null ; then \
		$(BIBTEX) $(BIBTEXFLAGS) $* ;\
		$(PDFLATEX) $(PDFLATEXFLAGS) $< ;\
	fi
	if grep 'Warning: Label(s) may' $*.log > /dev/null || \
			grep 'Rerun' $*.log > /dev/null || \
			grep 'Warning: Citation' $*.log > /dev/null; then \
		$(PDFLATEX) $(PDFLATEXFLAGS) $< ;\
	fi
	if grep 'Warning: Label(s) may' $*.log > /dev/null || \
			grep 'Rerun' $*.log > /dev/null || \
			grep 'Warning: Citation' $*.log > /dev/null; then \
		$(PDFLATEX) $(PDFLATEXFLAGS) $< ;\
	fi
	if grep 'Warning: Label(s) may' $*.log > /dev/null || \
			grep 'Rerun' $*.log > /dev/null || \
			grep 'Warning: Citation' $*.log > /dev/null; then \
		$(PDFLATEX) $(PDFLATEXFLAGS) $< ;\
	fi
	if grep 'Warning: Label(s) may' $*.log > /dev/null || \
			grep 'Rerun' $*.log > /dev/null || \
			grep 'Warning: Citation' $*.log > /dev/null; then \
		echo 'THE RESULT CONTAINS BROKEN REFERENCES, CITATIONS OR SOMETHING' ;\
	fi

# graphics for latex dvips
%.eps: %.png
	$(CONVERT) $< $(CONVERTFLAGS) $@

