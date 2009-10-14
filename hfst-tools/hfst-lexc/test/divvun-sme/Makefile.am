## Process this file with automake to produce Makefile.in
## Revision: $Id: Makefile.am,v 1.3 2009/08/31 13:21:36 tpirinen Exp $
MULTIFILE_TESTS=\
abbr-sme-lex.txt          noun-sme-lex.txt        punct-sme-lex.txt \
acro-sme-lex.txt          numeral-sme-lex.txt     sme-lex.txt \
adj-sme-lex.txt           particle-sme-lex.txt    subjunction-sme-lex.txt \
adv-sme-lex.txt           pp-sme-lex.txt          verb-sme-lex.txt \
conjunction-sme-lex.txt   pronoun-sme-lex.txt \
interjection-sme-lex.txt  propernoun-sme-lex.txt
TEST_FILES=$(MULTIFILE_TESTS)

MULTIFILE_TARGETS=\
	divvun-sme.lexc.hfst
MULTIFILE_WARGETS=\
	divvun-sme.lexc.hwfst
TEST_TARGETS=$(MULTIFILE_TARGETS) $(MULTIFILE_WARGETS)

MULTIFILE_PAIRS=\
	divvun-sme.pair-test
PAIR_FILES=$(MULTIFILE_PAIRS)

MULTIFILE_PAIRTARGETS=\
	divvun-sme.pair-results

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

divvun-sme.lexc.hfst: sme-lex.txt abbr-sme-lex.txt acro-sme-lex.txt adj-sme-lex.txt adv-sme-lex.txt conjunction-sme-lex.txt interjection-sme-lex.txt noun-sme-lex.txt numeral-sme-lex.txt particle-sme-lex.txt pp-sme-lex.txt pronoun-sme-lex.txt propernoun-sme-lex.txt punct-sme-lex.txt subjunction-sme-lex.txt verb-sme-lex.txt
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --unweighted --output=$@ $^

divvun-sme.lexc.hwfst: sme-lex.txt abbr-sme-lex.txt acro-sme-lex.txt adj-sme-lex.txt adv-sme-lex.txt conjunction-sme-lex.txt interjection-sme-lex.txt noun-sme-lex.txt numeral-sme-lex.txt particle-sme-lex.txt pp-sme-lex.txt pronoun-sme-lex.txt propernoun-sme-lex.txt punct-sme-lex.txt subjunction-sme-lex.txt verb-sme-lex.txt
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --weighted --output=$@ $^

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
	$(MAKE) HFSTLEXCFLAGS="-t" divvun-sme.lexc.hfst
	$(MAKE) HFSTLEXCFLAGS="-t" divvun-sme.lexc.hwfst
