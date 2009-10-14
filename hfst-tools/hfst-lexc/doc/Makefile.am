## Process this file with automake to produce Makefile.in
## Revision: $Id: Makefile.am,v 1.2 2009/05/04 08:59:33 tpirinen Exp $

HTML_TARGETS=README.html TODO.html HACKING.html tests.html
TWIKI_TARGETS=README.twiki TODO.twiki HACKING.twiki tests.twiki

.PHONY: doc apidoc twiki

doc: $(HTML_TARGETS) apidoc

twiki: $(TWIKI_TARGETS)

if CAN_DOXYGEN
apidoc: ../src/ Doxyconf
	$(DOXYGEN) Doxyconf
else !CAN_DOXYGEN
apidoc:
	@echo "Doxygen not available for apidoc building :-("
endif !CAN_DOXYGEN

%.rst: ../%
	$(LN_S) $< $@

if CAN_RST2HTML
%.html: %.rst
	$(PYTHON) $(RST2HTML) $< $@
else !CAN_RST2HTML
%.html: %.rst
	@echo "rst2html.py not available (install docutils)"
endif

if CAN_RST2XML
%.xml: %.rst
	$(PYTHON) $(RST2XML) $< $@
else !CAN_RST2XML
%.xml: %.rst
	@echo "rst2xml.py not available (install docutils)"
endif

if CAN_SAXON
%.twiki: %.xml
	$(SAXON) $< docutils2twiki.xslt |\
		sed -e 's/</\&lt;/g' -e 's/>/\&gt;/g' -e 's/\&lt;verbatim\&gt;/<verbatim>/g' -e 's/\&lt;\/verbatim\&gt;/<\/verbatim>/g' -e 's/&lt;!--\(.*\)/<!-- \1/g' -e 's/\(.*\)--\&gt;/\1-->/g' > $@
else !CAN_SAXON
%.twiki: %.xml
	@echo "xslt parser for docutils2twiki transformation missing (install saxon)"
endif

clean-local:
	-rm -f *.xml *.twiki *.html
