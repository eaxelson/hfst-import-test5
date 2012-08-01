## Makefile to autogenerate html pages from HFST 3 source tree and upload
## to project web space

SCP=scp
RST2HTML=rst2html.py

REMOTE_USER=mie
REMOTE_HOST=web.sourceforge.net
REMOTE_DIR=/home/project-web/hfst/htdocs/hfst3/

HFST3_PATH=../../hfst3
DOC_SOURCES=$(HFST3_PATH)/README $(HFST3_PATH)/README.deps \
			$(HFST3_PATH)/NEWS
DOC_TARGETS=README.html README.deps.html NEWS.html

all: $(DOC_TARGETS)

%.html: $(HFST3_PATH)/%
	$(RST2HTML) $< $@

%.html: $(HFST3_PATH)/doc/%.rst
	$(RST2HTML) $< $@

upload:
	$(SCP) $(DOC_TARGETS) $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)

