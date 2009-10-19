## Process this file with automake to produce Makefile.in
## Revision: $Id: Makefile.am,v 1.3 2009/08/31 13:21:36 tpirinen Exp $

MULTIFILE_TESTS=\
 fao-lex.txt \
	  noun-fao-lex.txt \
	  noun-fao-morph.txt \
	  propernoun-fao-lex.txt \
	  propernoun-fao-morph.txt \
	  adj-fao-lex.txt \
	  adj-fao-morph.txt \
	  verb-fao-lex.txt \
	  verb-fao-morph.txt \
	  adv-fao-lex.txt \
	  abbr-fao-lex.txt \
	  acro-fao-lex.txt \
	  pron-fao-lex.txt \
	  punct-fao-lex.txt \
	  numeral-fao-lex.txt \
	  pp-fao-lex.txt \
	  cc-fao-lex.txt \
	  cs-fao-lex.txt \
	  interj-fao-lex.txt \
	  det-fao-lex.txt

TEST_FILES=$(MULTIFILE_TESTS)

MULTIFILE_TARGETS=\
	fao.lexc.hfst
MULTIFILE_WARGETS=\
	fao.lexc.hwfst
TEST_TARGETS=$(MULTIFILE_TARGETS) $(MULTIFILE_WARGETS)

MULTIFILE_PAIRS=\
	fao.pair-test
PAIR_FILES=$(MULTIFILE_PAIRS)

MULTIFILE_PAIRTARGETS=\
	fao.pair-results

PAIR_TARGETS=$(MULTIFILE_PAIRTARGETS)

EXTRA_DIST=$(TEST_FILES) $(PAIR_FILES)

HFSTLEXC=$(top_builddir)/src/hfst-lexc
HFSTLEXCFLAGS=--verbose

.PHONY: valgrind time

$(HFSTLEXC):
	cd $(top_builddir)/src/ && $(MAKE) hfst-lexc

%.lexc.hfst: %.hlexc
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --unweighted --output=$@ $<

%.lexc.hwfst: %.hlexc
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --weighted --output=$@ $<

fao.lexc.hfst: fao-lex.txt  noun-fao-lex.txt noun-fao-morph.txt propernoun-fao-lex.txt propernoun-fao-morph.txt adj-fao-lex.txt adj-fao-morph.txt verb-fao-lex.txt verb-fao-morph.txt adv-fao-lex.txt abbr-fao-lex.txt acro-fao-lex.txt pron-fao-lex.txt punct-fao-lex.txt numeral-fao-lex.txt pp-fao-lex.txt cc-fao-lex.txt cs-fao-lex.txt interj-fao-lex.txt det-fao-lex.txt
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --unweighted --output=$@ $^

fao.lexc.hwfst: fao-lex.txt  noun-fao-lex.txt noun-fao-morph.txt propernoun-fao-lex.txt propernoun-fao-morph.txt adj-fao-lex.txt adj-fao-morph.txt verb-fao-lex.txt verb-fao-morph.txt adv-fao-lex.txt abbr-fao-lex.txt acro-fao-lex.txt pron-fao-lex.txt punct-fao-lex.txt numeral-fao-lex.txt pp-fao-lex.txt cc-fao-lex.txt cs-fao-lex.txt interj-fao-lex.txt det-fao-lex.txt
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --unweighted --output=$@ $^

clean-local:
	-rm -f $(TEST_TARGETS) $(PAIR_TARGETS)

check-local: $(HFSTLEXC) check-compile check-pair-test

check-compile: $(HFSTLEXC) $(TEST_FILES)
	$(MAKE) clean
	@echo "UNWEIGHTED - divvun Northern sami (this may take a while)... "
	@for f in $(MULTIFILE_TARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "WEIGHTED - divvun Northern sami (this may take a while)... "
	@for f in $(MULTIFILE_WARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"

if CAN_PAIRTEST
%.pair-results: %.pair-test %.lexc.hfst
	$(HFSTPAIRTEST) --input $*.lexc.hfst --pairs $*.pair-test --output $@

%.wair-results: %.pair-test %.lexc.hwfst
	$(HFSTPAIRTEST) --input $*.lexc.hfst --pairs $*.pair-test --output $@

check-pair-test: $(TEST_TARGETS) $(PAIR_FILES)
	@echo "UNWEIGHTED - Verifying test results..."
	for f in $(PAIR_TARGETS) ; do \
		$(MAKE) $$f || exit 1;\
	done
	@echo "OK"
	@echo "WEIGHTED - Verifying test results [MISSING]"
	@echo "XFAIL"

else !CAN_PAIRTEST
check-pair-test:
	@echo "Cannot verify results, please install hfst command line tools"

endif

time: clean
	$(MAKE) HFSTLEXCFLAGS="-t" fao.lexc.hfst
	$(MAKE) HFSTLEXCFLAGS="-t" fao.lexc.hwfst
