all:hfst-cla-2011.pdf

accuracy_%.pdf:correct_%_irc.tab correct_%_sms.tab
	R --quiet --no-save --args $^ $@ < draw_graphs.r
	sed -i -e 's/IRC_W/IRC W/g' $@

hfst-cla-2011.pdf:hfst-cla-2011.tex accuracy_first_guess.pdf \
                  accuracy_first_to_third_guess.pdf
	pdflatex $^
	bibtex $*
	pdflatex $^
	pdflatex $^

clean:
	-rm -f hfst-cla-2011.pdf hfst-cla-2011.aux hfst-cla-2011.bbl \
               hfst-cla-2011.blg accuracy_first_guess.pdf \
	       accuracy_first_to_third_guess.pdf
