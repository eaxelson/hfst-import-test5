## Process this file with automake to produce Makefile.in
## Author: *H*FST Team <hfst-bugs@helsinki.fi>
## Licence: GPL v3 (not newer)

TEXTFSTSRCS=tr1.txt tr2.txt compose.txt concatenate.txt conjunct.txt disjunct.txt \
tr_epsilons_removed.txt tr_determinized.txt tr_minimized.txt
OTHERSRCS=symbol_table1 symbol_table2

SRCS= $(TEXTFSTSRCS) $(OTHERSRCS)
EXTRA_DIST=$(SRCS)
check_DATA=$(SRCS)

#top_builddir=../../

# CRASH_TEST=./hfst-crash-test.sh


# these tools take two transducers as input and write one transducer as output
BINARY_TOOLS= \
	hfst-compose hfst-concatenate hfst-conjunct hfst-disjunct

check-local: test

# test harmonizing and input streams of different lenghts
hfst-%-functionality-test: tr1.hfst tr2.hfst tr1.hwfst tr2.hwfst symbol_table1

	$(top_builddir)/src/hfst-$* tr1.hfst tr2.hfst --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null
	diff TMP $(srcdir)/symbol_table1
	$(top_builddir)/src/hfst-$* tr1.hwfst tr2.hwfst --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null
	diff TMP $(srcdir)/symbol_table1
	cat tr1.hfst tr2.hfst | $(top_builddir)/src/hfst-$* -1 tr1.hfst | $(top_builddir)/src/hfst-compare -e $*.hfst 2>1 > /dev/null
	cat tr1.hfst tr2.hfst | $(top_builddir)/src/hfst-$* -2 tr1.hfst | $(top_builddir)/src/hfst-compare -e $*.hfst 2>1 > /dev/null
	cat tr1.hwfst tr2.hwfst | $(top_builddir)/src/hfst-$* -1 tr1.hwfst 2>1 | $(top_builddir)/src/hfst-compare -e $*.hwfst 2>1 > /dev/null
	cat tr1.hwfst tr2.hwfst | $(top_builddir)/src/hfst-$* -2 tr1.hwfst 2>1 | $(top_builddir)/src/hfst-compare -e $*.hwfst 2>1 > /dev/null


hfst-compare-functionality-test: tr1.hfst tr2.hfst tr1.hwfst tr2.hwfst symbol_table1 \
tr_epsilons_removed.hfst tr_determinized.hfst tr_minimized.hfst \
tr_epsilons_removed.hwfst tr_determinized.hwfst tr_minimized.hwfst

	cat tr1.hfst tr2.hfst | $(top_builddir)/src/hfst-compare -e tr1.hfst 2>1 > /dev/null
	cat tr1.hwfst tr2.hwfst | $(top_builddir)/src/hfst-compare -e tr1.hwfst 2>1 > /dev/null

	cat tr_epsilons_removed.hfst tr_determinized.hfst tr_minimized.hfst | \
	$(top_builddir)/src/hfst-compare -e tr1.hfst 2>1 > /dev/null

	cat tr_epsilons_removed.hwfst tr_determinized.hwfst tr_minimized.hwfst | \
	$(top_builddir)/src/hfst-compare -e tr1.hwfst 2>1 > /dev/null


test: fsts wfsts functionality

fsts: tr1.hfst tr2.hfst compose.hfst concatenate.hfst conjunct.hfst disjunct.hfst

wfsts: tr1.hwfst tr2.hwfst compose.hwfst concatenate.hwfst conjunct.hwfst disjunct.hwfst

clean-local:
	rm -f $(TEXTFSTSRCS:%.txt=%.hfst)
	rm -f $(TEXTFSTSRCS:%.txt=%.hwfst)
	rm -f TMP 1

functionality: $(BINARY_TOOLS:%=%-functionality-test) hfst-compare-functionality-test



tr1.hfst: tr1.txt symbol_table1
	$(top_builddir)/src/hfst-txt2fst -s -i $(srcdir)/tr1.txt -R $(srcdir)/symbol_table1 -o $@

tr2.hfst: tr2.txt symbol_table2
	$(top_builddir)/src/hfst-txt2fst -s -i $(srcdir)/tr2.txt -R $(srcdir)/symbol_table2 -o $@

tr1.hwfst: tr1.txt symbol_table1
	$(top_builddir)/src/hfst-txt2fst -s -i $(srcdir)/tr1.txt -R $(srcdir)/symbol_table1 -w -o $@

tr2.hwfst: tr2.txt symbol_table2
	$(top_builddir)/src/hfst-txt2fst -s -i $(srcdir)/tr2.txt -R $(srcdir)/symbol_table2 -w -o $@

%.hfst: %.txt
	$(top_builddir)/src/hfst-txt2fst -s -i $< -R $(srcdir)/symbol_table1 -o $@

%.hwfst: %.txt
	$(top_builddir)/src/hfst-txt2fst -s -i $< -R $(srcdir)/symbol_table1 -w -o $@


#crash: \
#	hfst-lookup-crash-test

# Test main functionality...

