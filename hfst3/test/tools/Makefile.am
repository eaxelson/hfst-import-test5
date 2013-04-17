## Process this file with automake to produce Makefile.in

## Copyright (C) 2010 University of Helsinki

## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

# programs to run for unit etc. testing
# prefer alphabetical order, except for core tests others depend upon
# If compare doesn't function 99 % of other tests will fail anyways

# test fsmbook-examples after establishing that simple tools work
if WANT_FSMBOOK_TESTS
SUBDIRS=. fsmbook-examples
endif

if WANT_STRESSTESTS
STRESSES=lexc2fst-stress.sh lookup-stress.sh proc-stress.sh valgrind.sh
endif
TESTS=hfst-check-version.sh hfst-check-help.sh hfst-check-environment.sh \
	  empty-input.sh \
	  permutate-file-params.sh \
	  compare-functionality.sh \
	  concatenate-functionality.sh \
	  compose-functionality.sh \
	  conjunct-functionality.sh \
	  disjunct-functionality.sh \
	  subtract-functionality.sh \
	  determinize-functionality.sh minimize-functionality.sh \
	  remove-epsilons-functionality.sh \
	  invert-functionality.sh project-functionality.sh \
	  prune-alphabet-functionality.sh repeat-functionality.sh \
	  reverse-functionality.sh reweight-functionality.sh \
	  substitute-functionality.sh \
	  strings2fst-functionality.sh fst2strings-functionality.sh\
	  txt2fst-functionality.sh fst2txt-functionality.sh \
	  summarize-functionality.sh proc-functionality.sh \
	  regexp2fst-functionality.sh \
	  split-functionality.sh head-functionality.sh tail-functionality.sh \
	  format-functionality.sh \
	  lookup-functionality.sh \
	  symbol-harmonization-functionality.sh \
	  $(STRESSES)

if WANT_LEXC
TESTS += lexc-functionality.sh
endif
if WANT_LEXC2FST
TESTS += lexc-compiler-functionality.sh
endif
if WANT_CALCULATE
TESTS += calculate-functionality.sh
endif
if WANT_SHUFFLE
TESTS += shuffle-functionality.sh
endif

# XFAIL_TESTS=incompatible-formats.sh latin-1-strings.sh

DEFAULT_CHECKS=0to3cats.hfst 2to4cats.hfst 4cats.hfst 4toINFcats.hfst \
		   cat2cat_or_CAT_uppercased.hfst cat2CAT_uppercased.hfst cat2dog.hfst \
		   cat+dog.hfst catdog.hfst cat_or_dog.hfst cat.hfst \
		   CAT_uppercased2cat.hfst CAT_uppercased.hfst cat_weight_final.hfst \
		   cat_weight_initial.hfst cat_weight_middle.hfst dog2cat.hfst \
		   dog.hfst infinitely_ambiguous.hfst infinitely_ambiguous_with_flags.hfst unification_flags.hfst non_minimal.hfst rule1.hfst \
		   rule2.hfst summarize_test1.hfst summarize_test2.hfst tac.hfst \
		   epsilon.hfst epsilon2cat.hfst cat2epsilon.hfst \
		   unknown-star.hfst identity-star.hfst unknown2a.hfst identity.hfst \
		   heavycat.hfst abid.hfst unk2unk.hfst unk_or_id_star.hfst a2b.hfst \
		   a2b_complement.hfst a2b_input_projection_complement.hfst empty.hfst \
		   walk_or_dog.hfst \
		   ab.hfst bc.hfst id.hfst aid.hfst idb.hfst \
		   ab_shuffle_bc.hfst id_shuffle_id.hfst aid_shuffle_idb.hfst \
		   prunable_alphabet.hfst non_prunable_alphabet_1.hfst non_prunable_alphabet_2.hfst id.hfst \
		   a2a_or_a2b_or_a2unk.hfst a2b_or_b2b_or_unk2b.hfst unk2unk_or_id.hfst \
		   a_or_id.hfst id_star_a_b_c.hfst
OL_CHECKS=cat2dog.hfstol cat2dog.genhfstol cat_weight_final.hfstol \
			proc-caps.hfstol proc-caps.genhfstol \
			compounds.hfstol compounds2.hfstol
if WANT_SFST
SFST_CHECKS=0to3cats.sfst 2to4cats.sfst 4cats.sfst\
			4toINFcats.sfst cat2cat_or_CAT_uppercased.sfst \
			cat2CAT_uppercased.sfst cat2dog.sfst \
			cat+dog.sfst catdog.sfst cat_or_dog.sfst \
			cat.sfst CAT_uppercased2cat.sfst CAT_uppercased.sfst\
			cat_weight_final.sfst cat_weight_initial.sfst \
			cat_weight_middle.sfst dog2cat.sfst dog.sfst \
			infinitely_ambiguous.sfst infinitely_ambiguous_with_flags.sfst unification_flags.sfst non_minimal.sfst \
			rule1.sfst rule2.sfst summarize_test1.sfst \
			summarize_test2.sfst tac.sfst unknown2a.sfst\
			identity.sfst abid.sfst unk2unk.sfst unk_or_id_star.sfst \
			a2b.sfst a2b_complement.sfst a2b_input_projection_complement.sfst \
			empty.sfst \
			walk_or_dog.sfst ab.sfst bc.sfst id.sfst aid.sfst idb.sfst \
			ab_shuffle_bc.sfst id_shuffle_id.sfst aid_shuffle_idb.sfst \
			prunable_alphabet.sfst non_prunable_alphabet_1.sfst non_prunable_alphabet_2.sfst id.sfst \
			a2a_or_a2b_or_a2unk.sfst a2b_or_b2b_or_unk2b.sfst unk2unk_or_id.sfst \
			a_or_id.sfst id_star_a_b_c.sfst
endif
if WANT_OPENFST
OFST_CHECKS=0to3cats.ofst \
			2to4cats.ofst 4cats.ofst 4toINFcats.ofst \
			cat2cat_or_CAT_uppercased.ofst cat2CAT_uppercased.ofst \
			cat2dog.ofst cat+dog.ofst catdog.ofst \
			cat_or_dog.ofst cat.ofst CAT_uppercased2cat.ofst \
			CAT_uppercased.ofst cat_weight_final.ofst \
			cat_weight_initial.ofst cat_weight_middle.ofst \
			dog2cat.ofst dog.ofst infinitely_ambiguous.ofst infinitely_ambiguous_with_flags.ofst unification_flags.ofst \
			non_minimal.ofst rule1.ofst rule2.ofst \
			summarize_test1.ofst summarize_test2.ofst tac.ofst\
			unknown2a.ofst identity.ofst \
			heavycat.hfst abid.ofst unk2unk.ofst \
			cat2dog_0.3.ofst cat2dog_0.5.ofst empty.ofst unk_or_id_star.ofst \
			a2b.ofst a2b_complement.ofst a2b_input_projection_complement.ofst \
			walk_or_dog.ofst ab.ofst bc.ofst id.ofst aid.ofst idb.ofst \
			ab_shuffle_bc.ofst id_shuffle_id.ofst aid_shuffle_idb.ofst \
			prunable_alphabet.ofst non_prunable_alphabet_1.ofst non_prunable_alphabet_2.ofst id.ofst \
			a2a_or_a2b_or_a2unk.ofst a2b_or_b2b_or_unk2b.ofst unk2unk_or_id.ofst \
			a_or_id.ofst id_star_a_b_c.ofst
endif
if WANT_FOMA
FOMA_CHECKS=0to3cats.foma 2to4cats.foma 4cats.foma \
			4toINFcats.foma cat2cat_or_CAT_uppercased.foma \
			cat2CAT_uppercased.foma cat2dog.foma \
			cat+dog.foma catdog.foma cat_or_dog.foma \
			cat.foma CAT_uppercased2cat.foma \
			CAT_uppercased.foma cat_weight_final.foma \
			cat_weight_initial.foma cat_weight_middle.foma \
			dog2cat.foma dog.foma infinitely_ambiguous.foma infinitely_ambiguous_with_flags.foma unification_flags.foma \
			non_minimal.foma rule1.foma rule2.foma \
			summarize_test1.foma summarize_test2.foma tac.foma\
			unknown2a.foma identity.foma abid.foma unk2unk.foma unk_or_id_star.foma \
			a2b.foma a2b_complement.foma a2b_input_projection_complement.foma \
			empty.foma \
			walk_or_dog.foma ab.foma bc.foma id.foma aid.foma idb.foma \
			ab_shuffle_bc.foma id_shuffle_id.foma aid_shuffle_idb.foma \
			prunable_alphabet.foma non_prunable_alphabet_1.foma non_prunable_alphabet_2.foma id.foma \
			a2a_or_a2b_or_a2unk.foma a2b_or_b2b_or_unk2b.foma unk2unk_or_id.foma \
			a_or_id.foma id_star_a_b_c.foma
endif

check_DATA=$(DEFAULT_CHECKS) $(SFST_CHECKS) $(OFST_CHECKS) $(FOMA_CHECKS) $(OL_CHECKS)

FST_TXTS=0to3cats.txt 2to4cats.txt 4cats.txt 4toINFcats.txt \
		 cat2cat_or_CAT_uppercased.txt cat2CAT_uppercased.txt \
		 cat2epsilon.txt cat2dog.txt \
		 cat+dog.txt catdog.txt cat_or_dog.txt cat.txt CAT_uppercased2cat.txt \
		 CAT_uppercased.txt cat_weight_final.txt cat_weight_initial.txt \
		 cat_weight_middle.txt dog2cat.txt dog.txt\
		 epsilon.txt epsilon2cat.txt infinitely_ambiguous.txt infinitely_ambiguous_with_flags.txt unification_flags.txt \
		 non_minimal.txt rule1.txt rule2.txt summarize_test1.txt \
		 summarize_test2.txt tac.txt proc-caps.txt \
		 unknown-star.txt identity-star.txt unknown2a.txt identity.txt\
		 heavycat.txt abid.txt unk2unk.txt cat2dog_0.3.txt cat2dog_0.5.txt empty.txt unk_or_id_star.txt \
		 a2b.txt a2b_complement.txt a2b_input_projection_complement.txt \
		 walk_or_dog.txt ab.txt bc.txt id.txt aid.txt idb.txt \
		 ab_shuffle_bc.txt id_shuffle_id.txt aid_shuffle_idb.txt \
		 compounds.txt compounds2.txt \
		 id_shuffle_id.txt aid_shuffle_idb.txt \
		 prunable_alphabet.txt non_prunable_alphabet_1.txt non_prunable_alphabet_2.txt id.txt unk2unk_or_id.txt \
		 a2a_or_a2b_or_a2unk.txt a2b_or_b2b_or_unk2b.txt a_or_id.txt id_star_a_b_c.txt
FST_STRINGS=cat.strings proc-caps-in.strings proc-caps-gen.strings \
			proc-caps-out1.strings proc-caps-out2.strings \
			proc-caps-out3.strings proc-caps-out4.strings \
			proc-caps-out5.strings \
			proc-cat-out.strings proc-cat-weighted-out.strings \
			proc-compounds-out.strings proc-compounds.strings \
			proc-compounds2-out.strings proc-compounds2.strings \
			utf-8.strings latin-1.strings \
			cat2dog.strings heavycat.strings
FST_PAIRS=cat2dog.pairs
FST_PAIRSTRINGS=cat2dog.pairstring
FST_SPACESTRINGS=cat2dog.spaces
SUBSTITUTE_TXTS=cat2dog.substitute
XRE_TXTS=cats_and_dogs.xre cats_and_dogs_semicolon.xre \
	 at_file_quote.openfst-tropical.xre \
	 at_file_quote.sfst.xre at_file_quote.foma.xre
LEXC_TXTS=basic.cat-dog-bird.lexc basic.colons.lexc basic.comments.lexc \
		  basic.empty-sides.lexc basic.end.lexc basic.escapes.lexc \
		  basic.infostrings.lexc basic.initial-lexicon-empty.lexc \
		  basic.multichar-symbols.lexc basic.multichar-symbol-with-0.lexc \
		  basic.multi-entry-lines.lexc basic.multi-file-1.lexc \
		  basic.multi-file-2.lexc basic.multi-file-3.lexc \
		  basic.no-newline-at-end.lexc basic.no-Root.lexc \
		  basic.punctuation.lexc basic.root-loop.lexc \
		  basic.spurious-lexicon.lexc basic.string-pairs.lexc \
		  basic.two-lexicons.lexc basic.UTF-8.lexc basic.zeros-epsilons.lexc \
		  cat.lexc hfst.weights.lexc stress.random-lexicons-100.lexc \
		  xfail.bogus.lexc xfail.ISO-8859-1.lexc xfail.lexicon-semicolon.lexc \
		  xre.any-variations.lexc xre.at-file.lexc \
		  xre.automatic-multichar-symbols.lexc xre.basic.lexc \
		  xre.definitions.lexc xre.months.lexc xre.nested-definitions.lexc \
		  xre.numeric-star.lexc xre.sharp.lexc xre.quotations.lexc \
		  xre.star-plus-optional.lexc basic.lowercase-lexicon-end.lexc \
		  basic.multichar-flag-with-zero.lexc
#XFST_TXTS=cat-regex.xfst
ALL_SRCS=$(FST_TXTS) $(FST_STRINGS) $(FST_PAIRS) $(FST_PAIRSTRINGS) \
		$(FST_SPACESTRINGS) $(SUBSTITUTE_TXTS) $(XRE_TEXTS)

# this compilation is used by check target before running the tests
#%.hfst: %.txt
.txt.hfst:
	${top_builddir}/tools/src/hfst-txt2fst -e '@0@' $< > $@

#%.sfst: %.txt
.txt.sfst:
	${top_builddir}/tools/src/hfst-txt2fst -f sfst -e '@0@' $< > $@

#%.ofst: %.txt
.txt.ofst:
	${top_builddir}/tools/src/hfst-txt2fst -f openfst-tropical -e '@0@' $< > $@

#%.foma: %.txt
.txt.foma:
	${top_builddir}/tools/src/hfst-txt2fst -f foma -e '@0@' $< > $@

#%.hfstol: %.txt
.txt.hfstol:
	${top_builddir}/tools/src/hfst-txt2fst -e '@0@' -i $< | ${top_builddir}/tools/src/hfst-fst2fst -w -o $@

#%.genhfstol: %.txt
.txt.genhfstol:
	${top_builddir}/tools/src/hfst-txt2fst -e '@0@' -i $< | ${top_builddir}/tools/src/hfst-invert | ${top_builddir}/tools/src/hfst-fst2fst -w -o $@

EXTRA_FILES=cat2dog.substitute cat.strings cat.txt cats_and_dogs.xre utf-8.strings latin-1.strings c_a_t.strings dos.strings not-contains-a.xre not-contains-a-comment-emptyline.xre parallel-left-arrow-multicom-emptyline.xre  parallel-left-arrow.xre 

# files needed for test programs
EXTRA_DIST=empty-file $(FST_TXTS) $(FST_STRINGS) $(FST_PAIRS) $(FST_PAIRSTRINGS) $(FST_SPACESTRINGS) $(SUBSTITUTE_TXTS) $(XRE_TXTS) $(XFST_TXTS) $(TESTS) $(EXTRA_FILES) $(LEXC_TXTS) script.xfst

valgrind: $(CHECK_DATA)
	$(srcdir)/valgrind.sh $(srcdir)

clean-local:
	-rm -f $(check_DATA)
