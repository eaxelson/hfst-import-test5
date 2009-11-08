## Process this file with automake to produce Makefile.in
## Revision: $Id: Makefile.am,v 1.2 2009/08/31 13:21:36 tpirinen Exp $

BASIC_TESTS=\
	morphalou.hlexc
TEST_FILES=$(BASIC_TESTS)

BASIC_TARGETS=\
	morphalou.lexc.hfst
BASIC_WARGETS=\
	morphalou.lexc.hwfst
TEST_TARGETS=$(BASIC_TARGETS) $(BASIC_WARGETS)

BASIC_PAIRS=\
	morphalou.pair-test
PAIR_FILES=$(BASIC_PAIRS)

BASIC_PAIRTARGETS=\
	morphalou.pair-results

PAIR_TARGETS=$(BASIC_PAIRTARGETS)

# we cannot EXTRA_DIST morphalou because of non-compatible licence
EXTRA_DIST=$(PAIR_FILES) empty.hlexc

HFSTLEXC=$(top_builddir)/src/hfst-lexc
HFSTLEXCFLAGS=--verbose

$(HFSTLEXC):
	cd $(top_builddir)/src/ && $(MAKE) hfst-lexc

morphalou.hlexc:
	@if test \! -f $@ ; then \
		cp $(srcdir)/empty.hlexc $@ ; \
	fi
	touch $@

distclean-local:
	-rm -f morphalou.hlexc

%.lexc.hfst: %.hlexc
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --unweighted --output=$@ $<

%.lexc.hwfst: %.hlexc
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --weighted --output=$@ $<

clean-local:
	-rm -f $(TEST_TARGETS) $(PAIR_TARGETS)

check-local: $(HFSTLEXC) check-compile check-pair-test

check-compile: $(HFSTLEXC) $(TEST_FILES)
	$(MAKE) clean
	@echo "UNWEIGHTED - morphalou... "
	@for f in $(BASIC_TARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "WEIGHTED - morphalou... "
	@for f in $(BASIC_WARGETS) ; do\
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

