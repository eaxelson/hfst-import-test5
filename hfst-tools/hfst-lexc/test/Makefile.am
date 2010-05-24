## Process this file with automake to produce Makefile.in
## Revision: $Id: Makefile.am,v 1.6 2009/08/31 13:21:36 tpirinen Exp $
SUBDIRS=. kos83 omorfi divvun-sme morphalou fao
BASIC_TESTS=\
	basic.cat-dog-bird.hlexc basic.colons.hlexc basic.comments.hlexc \
	basic.empty-sides.hlexc basic.end.hlexc basic.escapes.hlexc \
	basic.infostrings.hlexc \
	basic.initial-lexicon-empty.hlexc basic.multichar-symbols.hlexc \
	basic.multi-entry-lines.hlexc \
	basic.no-Root.hlexc basic.punctuation.hlexc basic.root-loop.hlexc \
	basic.spurious-lexicon.hlexc basic.string-pairs.hlexc \
	basic.two-lexicons.hlexc basic.UTF-8.hlexc basic.zeros-epsilons.hlexc \
	basic.no-newline-at-end.hlexc basic.multichar-symbol-with-0.hlexc
MULTIFILE_TESTS=\
	basic.multi-file-1.hlexc basic.multi-file-2.hlexc basic.multi-file-3.hlexc
MY_EXT_TESTS=\
	hlexc.weights.hlexc
STRESS_TESTS=\
	stress.random-lexicons-100.hlexc
VALGRIND_TESTS=\
	valgrind.minimal.hlexc valgrind.medium.hlexc
XFAIL_TESTS=\
	xfail.bogus.hlexc xfail.ISO-8859-1.hlexc \
	xfail.lexicon-semicolon.hlexc 
XRE_TESTS=\
	xre.at-file.hlexc xre.automatic-multichar-symbols.hlexc \
	xre.any-variations.hlexc xre.basic.hlexc \
	xre.definitions.hlexc xre.months.hlexc \
	xre.nested-definitions.hlexc xre.numeric-star.hlexc \
	xre.sharp.hlexc xre.star-plus-optional.hlexc
TEST_FILES=$(BASIC_TESTS) $(MULTIFILE_TESTS) $(MY_EXT_TESTS) $(STRESS_TESTS)\
	$(VALGRIND_TESTS) \
	$(XFAIL_TESTS) $(XRE_TESTS)

# copy paste tests and replace hlexc => lexc.hfst unless special rules apply!
# (s/FILES/TARGETS/g s/\.hlexc/.lexc.hfst/g)
BASIC_TARGETS=\
	basic.cat-dog-bird.lexc.hfst basic.colons.lexc.hfst basic.comments.lexc.hfst \
	basic.empty-sides.lexc.hfst basic.end.lexc.hfst basic.escapes.lexc.hfst \
	basic.infostrings.lexc.hfst \
	basic.initial-lexicon-empty.lexc.hfst basic.multichar-symbols.lexc.hfst \
	basic.multi-entry-lines.lexc.hfst \
	basic.no-Root.lexc.hfst basic.punctuation.lexc.hfst basic.root-loop.lexc.hfst \
	basic.spurious-lexicon.lexc.hfst basic.string-pairs.lexc.hfst \
	basic.two-lexicons.lexc.hfst basic.UTF-8.lexc.hfst basic.zeros-epsilons.lexc.hfst \
	basic.no-newline-at-end.lexc.hfst basic.multichar-symbol-with-0.lexc.hfst
MULTIFILE_TARGETS=\
	basic.multi-file.lexc.hfst
MY_EXT_TARGETS=\
	hlexc.weights.lexc.hfst
STRESS_TARGETS=\
	stress.random-lexicons-100.lexc.hfst
VALGRIND_TARGETS=\
	valgrind.minimal.lexc.hfst valgrind.medium.lexc.hfst
XFAIL_TARGETS=\
	xfail.bogus.lexc.hfst xfail.ISO-8859-1.lexc.hfst \
	xfail.lexicon-semicolon.lexc.hfst 
XRE_TARGETS=\
	xre.at-file.lexc.hfst xre.automatic-multichar-symbols.lexc.hfst \
	xre.any-variations.lexc.hfst xre.basic.lexc.hfst \
	xre.definitions.lexc.hfst xre.months.lexc.hfst \
	xre.nested-definitions.lexc.hfst xre.numeric-star.lexc.hfst \
	xre.sharp.lexc.hfst xre.star-plus-optional.lexc.hfst
BASIC_WARGETS=\
	basic.cat-dog-bird.lexc.hwfst basic.colons.lexc.hwfst basic.comments.lexc.hwfst \
	basic.empty-sides.lexc.hwfst basic.end.lexc.hwfst basic.escapes.lexc.hwfst \
	basic.infostrings.lexc.hwfst \
	basic.initial-lexicon-empty.lexc.hwfst basic.multichar-symbols.lexc.hwfst \
	basic.multi-entry-lines.lexc.hwfst \
	basic.no-Root.lexc.hwfst basic.punctuation.lexc.hwfst basic.root-loop.lexc.hwfst \
	basic.spurious-lexicon.lexc.hwfst basic.string-pairs.lexc.hwfst \
	basic.two-lexicons.lexc.hwfst basic.UTF-8.lexc.hwfst basic.zeros-epsilons.lexc.hwfst \
	basic.no-newline-at-end.lexc.hwfst basic.multichar-symbol-with-0.lexc.hwfst
MULTIFILE_WARGETS=\
	basic.multi-file.lexc.hwfst
MY_EXT_WARGETS=\
	hlexc.weights.lexc.hwfst
STRESS_WARGETS=\
	stress.random-lexicons-100.lexc.hwfst
VALGRIND_WARGETS=\
	valgrind.minimal.lexc.hwfst valgrind.medium.lexc.hwfst
XFAIL_WARGETS=\
	xfail.bogus.lexc.hwfst xfail.ISO-8859-1.lexc.hwfst \
	xfail.lexicon-semicolon.lexc.hwfst 
XRE_WARGETS=\
	xre.at-file.lexc.hwfst xre.automatic-multichar-symbols.lexc.hwfst \
	xre.any-variations.lexc.hwfst xre.basic.lexc.hwfst \
	xre.definitions.lexc.hwfst xre.months.lexc.hwfst \
	xre.nested-definitions.lexc.hwfst xre.numeric-star.lexc.hwfst \
	xre.sharp.lexc.hwfst xre.star-plus-optional.lexc.hwfst
TEST_TARGETS=$(BASIC_TARGETS) $(MULTIFILE_TARGETS) $(MY_EXT_TARGETS) \
	$(STRESS_TARGETS) $(XRE_TARGETS) \
	$(BASIC_WARGETS) $(MULTIFILE_WARGETS) $(MY_EXT_WARGETS) \
	$(STRESS_WARGETS) $(XRE_WARGETS)

BASIC_PAIRS=\
	basic.cat-dog-bird.pair-test basic.colons.pair-test basic.comments.pair-test \
	basic.empty-sides.pair-test basic.end.pair-test basic.escapes.pair-test \
	basic.infostrings.pair-test \
	basic.initial-lexicon-empty.pair-test basic.multichar-symbols.pair-test \
	basic.multi-entry-lines.pair-test \
	basic.no-Root.pair-test basic.punctuation.pair-test basic.root-loop.pair-test \
	basic.spurious-lexicon.pair-test basic.string-pairs.pair-test \
	basic.two-lexicons.pair-test basic.UTF-8.pair-test basic.zeros-epsilons.pair-test \
	basic.no-newline-at-end.pair-test basic.multichar-symbol-with-0.pair-test
MULTIFILE_PAIRS=\
	basic.multi-file.pair-test
MY_EXT_PAIRS=\
	hlexc.weights.pair-test
STRESS_PAIRS=\
	stress.random-lexicons-100.pair-test
VALGRIND_PAIRS=\
	valgrind.minimal.pair-test valgrind.medium.pair-test
XFAIL_PAIRS=\
	xfail.bogus.pair-test xfail.ISO-8859-1.pair-test \
	xfail.lexicon-semicolon.pair-test 
XRE_PAIRS=\
	xre.at-file.pair-test xre.automatic-multichar-symbols.pair-test \
	xre.any-variations.pair-test xre.basic.pair-test \
	xre.definitions.pair-test xre.months.pair-test \
	xre.nested-definitions.pair-test xre.numeric-star.pair-test \
	xre.sharp.pair-test xre.star-plus-optional.pair-test
PAIR_FILES=$(BASIC_PAIRS) $(MULTIFILE_PAIRS) $(MY_EXT_PAIRS) $(STRESS_PAIRS)\
	$(VALGRIND_PAIRS) \
	$(XFAIL_PAIRS) $(XRE_PAIRS)

BASIC_PAIRTARGETS=\
	basic.cat-dog-bird.pair-results basic.colons.pair-results basic.comments.pair-results \
	basic.empty-sides.pair-results basic.end.pair-results basic.escapes.pair-results \
	basic.infostrings.pair-results \
	basic.initial-lexicon-empty.pair-results basic.multichar-symbols.pair-results \
	basic.multi-entry-lines.pair-results \
	basic.no-Root.pair-results basic.punctuation.pair-results basic.root-loop.pair-results \
	basic.spurious-lexicon.pair-results basic.string-pairs.pair-results \
	basic.two-lexicons.pair-results basic.UTF-8.pair-results basic.zeros-epsilons.pair-results \
	basic.no-newline-at-end.pair-results basic.multichar-symbol-with-0.pair-results
MULTIFILE_PAIRTARGETS=\
	basic.multi-file.pair-results
MY_EXT_PAIRTARGETS=\
	hlexc.weights.pair-results
STRESS_PAIRTARGETS=\
	stress.random-lexicons-100.pair-results
VALGRIND_PAIRTARGETS=\
	valgrind.minimal.pair-results valgrind.medium.pair-results
XFAIL_PAIRTARGETS=\
	xfail.bogus.pair-results xfail.ISO-8859-1.pair-results \
	xfail.lexicon-semicolon.pair-results 
XRE_PAIRTARGETS=\
	xre.at-file.pair-results xre.automatic-multichar-symbols.pair-results \
	xre.any-variations.pair-results xre.basic.pair-results \
	xre.definitions.pair-results xre.months.pair-results \
	xre.nested-definitions.pair-results xre.numeric-star.pair-results \
	xre.sharp.pair-results xre.star-plus-optional.pair-results

PAIR_TARGETS=$(BASIC_PAIRTARGETS) $(MULTIFILE_PAIRTARGETS) \
	$(MY_EXT_PAIRTARGETS) $(STRESS_PAIRTARGETS) $(XRE_PAIRTARGETS)

TEST_BINARIES=a.hfst b.hfst
TEST_WINARIES=a.hwfst b.hwfst
BINARIES_FILES=$(TEST_BINARIES) $(TEST_WINARIES)

EXTRA_DIST=$(TEST_FILES) $(PAIR_FILES) $(BINARIES_FILES)
check_DATA=$(TEST_FILES) $(PAIR_FILES) $(BINARIES_FILES)

HFSTLEXC=$(top_builddir)/src/hfst-lexc
HFSTLEXCFLAGS=--verbose

.PHONY: valgrind

$(HFSTLEXC):
	cd $(top_builddir)/src/ && $(MAKE) hfst-lexc

%.lexc.hfst: %.hlexc
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --unweighted --output=$@ $<

%.lexc.hwfst: %.hlexc
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --weighted --output=$@ $<

basic.multi-file.lexc.hfst: basic.multi-file-1.hlexc basic.multi-file-2.hlexc basic.multi-file-3.hlexc
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --unweighted --output=$@ $^

basic.multi-file.lexc.hwfst: basic.multi-file-1.hlexc basic.multi-file-2.hlexc basic.multi-file-3.hlexc
	$(HFSTLEXC) $(HFSTLEXCFLAGS) --weighted --output=$@ $^

xre.at-file.lexc.hwfst: xre.at-file.hlexc
	sed -e 's/hfst/hwfst/' $< | $(HFSTLEXC) $(HFSTLEXCFLAGS) --weighted --output $@

hlexc.xre.at-file.lexc.hwfst: hlexc.xre.at-file.hlexc
	sed -e 's/hfst/hwfst/' $< | $(HFSTLEXC) $(HFSTLEXCFLAGS) --weighted --output $@

if CAN_VALGRIND
valgrind: $(HFSTLEXC) $(VALGRIND_TESTS)
	$(VALGRIND) --leak-check=full -v --log-file=smallgrind.out $(HFSTLEXC) $(HFSTLEXCFLAGS) --output=/dev/null valgrind.minimal.hlexc
	$(VALGRIND) --leak-check=full -v --log-file=mediumgrind.out $(HFSTLEXC) $(HFSTLEXCFLAGS) --output=/dev/null valgrind.medium.hlexc

cachegrind: $(HFSTLEXC) $(VALGRIND_TESTS)
	$(VALGRIND) --tool=cachegrind $(HFSTLEXC) --quiet --output=/dev/null valgrind.minimal.hlexc
	$(VALGRIND) --tool=cachegrind $(HFSTLEXC) --quiet --output=/dev/null valgrind.medium.hlexc

callgrind: $(HFSTLEXC) $(VALGRIND_TESTS)
	$(VALGRIND) --tool=callgrind $(HFSTLEXC) --quiet --output=/dev/null valgrind.minimal.hlexc
	$(VALGRIND) --tool=callgrind $(HFSTLEXC) --quiet --output=/dev/null valgrind.medium.hlexc

else !CAN_VALGRIND
valgrind:
	@echo "Valgrind missing, install valgrind"

callgrind:
	@echo "Valgrind missing, install valgrind"

cachegrind:
	@echo "Valgrind missing, install valgrind"
endif

clean-local:
	-rm -f $(TEST_TARGETS) $(PAIR_TARGETS) \
		valgrind.out* cachegrind.out* callgrind.out*

check-local: $(HFSTLEXC) check-command-line check-compile check-pair-test

check-command-line: $(HFSTLEXC)
	@echo -n "--version... "
	@$(HFSTLEXC) --version > /dev/null
	@echo "OK"
	@echo -n "--help... "
	@$(HFSTLEXC) --help > /dev/null
	@echo "OK"
	@echo -n "INFILE param "
	@$(HFSTLEXC) --output=/dev/null basic.cat-dog-bird.hlexc
	@echo "OK"
	@echo -n "PIPE INFILE "
	@cat basic.cat-dog-bird.hlexc | $(HFSTLEXC) --output=/dev/null
	@echo "OK"

check-compile: $(HFSTLEXC) $(TEST_FILES)
	$(MAKE) clean
	@echo "UNWEIGHTED - basics... "
	@for f in $(BASIC_TARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "UNWEIGHTED - multifiles... "
	@for f in $(MULTIFILE_TARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "UNWEIGHTED - stress testing (may take a while)... "
	@for f in $(STRESS_TARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "UNWEIGHTED - Xerox style regular expressions... "
	@for f in $(XRE_TARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "UNWEIGHTED - HFST lexc extensions... "
	@for f in $(MY_EXT_TARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "UNWEIGHTED - expected failures (watch for segfaults manually)... "
	@for f in $(XFAIL_TARGETS) ; do\
		$(MAKE) -s -i HFSTLEXCFLAGS="-q" $$f ;\
	done
	@echo "WEIGHTED - basics... "
	@for f in $(BASIC_WARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "WEIGHTED - multifiles... "
	@for f in $(MULTIFILE_WARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "WEIGHTED - stress testing (may take a while... "
	@for f in $(STRESS_WARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "WEIGHTED - Xerox style regular expressions... "
	@for f in $(XRE_WARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "WEIGHTED - HFST lexc extensions... "
	@for f in $(MY_EXT_WARGETS) ; do\
		$(MAKE) -s HFSTLEXCFLAGS="-q" $$f || exit 1 ;\
	done
	@echo "OK"
	@echo "WEIGHTED - expected failures (watch for segfaults manually)... "
	@for f in $(XFAIL_WARGETS) ; do\
		$(MAKE) -s -i HFSTLEXCFLAGS="-q" $$f ;\
	done

if CAN_PAIRTEST
%.pair-results: %.pair-test %.lexc.hfst
	$(HFSTPAIRTEST) --input $*.lexc.hfst --pairs $*.pair-test --output $@

%.wair-results: %.pair-test %.lexc.hwfst
	$(HFSTPAIRTEST) --input $*.lexc.hfst --pairs $*.pair-test --output $@

basic.colons.pair-results:
	@echo "XFAIL: hfst-pair-test unsupports escaped colons"

basic.punctuation.pair-results:
	@echo "XFAIL: hfst-pair-test unsupports escaped colons"

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

