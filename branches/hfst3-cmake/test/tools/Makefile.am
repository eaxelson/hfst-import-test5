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
if WANT_STRESSTESTS
STRESSES=lexc2fst-stress.sh lookup-stress.sh proc-stress.sh
endif
TESTS=hfst-check-version.sh hfst-check-help.sh empty-input.sh \
	  permutate-file-params.sh \
	  compare-functionality.sh \
	  concatenate-functionality.sh \
	  compose-functionality.sh \
	  conjunct-functionality.sh \
	  disjunct-functionality.sh \
	  subtract-functionality.sh \
	  determinize-functionality.sh minimize-functionality.sh \
	  remove-epsilons-functionality.sh \
	  invert-functionality.sh project-functionality.sh repeat-functionality.sh \
	  reverse-functionality.sh substitute-functionality.sh \
	  strings2fst-functionality.sh fst2strings-functionality.sh\
	  txt2fst-functionality.sh fst2txt-functionality.sh \
	  summarize-functionality.sh proc-functionality.sh \
	  regexp2fst-functionality.sh \
	  split-functionality.sh head-functionality.sh tail-functionality.sh \
	  calculate-functionality.sh format-functionality.sh \
	  lookup-functionality.sh \
	  lexc-functionality.sh \
	  lexc-compiler-functionality.sh \
	  utf-8-strings.sh latin-1-strings.sh \
	  $(STRESSES)
DEFAULT_CHECKS=0to3cats.hfst 2to4cats.hfst 4cats.hfst 4toINFcats.hfst \
		   cat2cat_or_CAT_uppercased.hfst cat2CAT_uppercased.hfst cat2dog.hfst \
		   cat+dog.hfst catdog.hfst cat_or_dog.hfst cat.hfst \
		   CAT_uppercased2cat.hfst CAT_uppercased.hfst cat_weight_final.hfst \
		   cat_weight_initial.hfst cat_weight_middle.hfst dog2cat.hfst \
		   dog.hfst infinitely_ambiguous.hfst non_minimal.hfst rule1.hfst \
		   rule2.hfst summarize_test1.hfst summarize_test2.hfst tac.hfst \
		   epsilon.hfst epsilon2cat.hfst cat2epsilon.hfst \
		   unknown-star.hfst identity-star.hfst unknown2a.hfst identity.hfst \
		   heavycat.hfst abid.hfst unk2unk.hfst unk_or_id_star.hfst a2b.hfst \
		   a2b_complement.hfst a2b_input_projection_complement.hfst empty.hfst \
		   walk_or_dog.hfst
OL_CHECKS=cat2dog.hfst.ol cat2dog.gen.hfst.ol cat_weight_final.hfst.ol \
			proc-caps.hfst.ol proc-caps.gen.hfst.ol
if WANT_SFST
SFST_CHECKS=0to3cats.hfst.sfst 2to4cats.hfst.sfst 4cats.hfst.sfst\
			4toINFcats.hfst.sfst cat2cat_or_CAT_uppercased.hfst.sfst \
			cat2CAT_uppercased.hfst.sfst cat2dog.hfst.sfst \
			cat+dog.hfst.sfst catdog.hfst.sfst cat_or_dog.hfst.sfst \
			cat.hfst.sfst CAT_uppercased2cat.hfst.sfst CAT_uppercased.hfst.sfst\
			cat_weight_final.hfst.sfst cat_weight_initial.hfst.sfst \
			cat_weight_middle.hfst.sfst dog2cat.hfst.sfst dog.hfst.sfst \
			infinitely_ambiguous.hfst.sfst non_minimal.hfst.sfst \
			rule1.hfst.sfst rule2.hfst.sfst summarize_test1.hfst.sfst \
			summarize_test2.hfst.sfst tac.hfst.sfst unknown2a.hfst.sfst\
			identity.hfst.sfst abid.hfst.sfst unk2unk.hfst.sfst unk_or_id_star.hfst.sfst \
			a2b.hfst.sfst a2b_complement.hfst.sfst a2b_input_projection_complement.hfst.sfst \
			empty.hfst.sfst \
			walk_or_dog.hfst.sfst
endif
if WANT_OPENFST
OFST_CHECKS=0to3cats.hfst.ofst \
			2to4cats.hfst.ofst 4cats.hfst.ofst 4toINFcats.hfst.ofst \
			cat2cat_or_CAT_uppercased.hfst.ofst cat2CAT_uppercased.hfst.ofst \
			cat2dog.hfst.ofst cat+dog.hfst.ofst catdog.hfst.ofst \
			cat_or_dog.hfst.ofst cat.hfst.ofst CAT_uppercased2cat.hfst.ofst \
			CAT_uppercased.hfst.ofst cat_weight_final.hfst.ofst \
			cat_weight_initial.hfst.ofst cat_weight_middle.hfst.ofst \
			dog2cat.hfst.ofst dog.hfst.ofst infinitely_ambiguous.hfst.ofst \
			non_minimal.hfst.ofst rule1.hfst.ofst rule2.hfst.ofst \
			summarize_test1.hfst.ofst summarize_test2.hfst.ofst tac.hfst.ofst\
			unknown2a.hfst.ofst identity.hfst.ofst \
			heavycat.hfst abid.hfst.ofst unk2unk.hfst.ofst \
			cat2dog_0.3.hfst.ofst cat2dog_0.5.hfst.ofst empty.hfst.ofst unk_or_id_star.hfst.ofst \
			a2b.hfst.ofst a2b_complement.hfst.ofst a2b_input_projection_complement.hfst.ofst \
			walk_or_dog.hfst.ofst
endif
if WANT_FOMA
FOMA_CHECKS=0to3cats.hfst.foma 2to4cats.hfst.foma 4cats.hfst.foma \
			4toINFcats.hfst.foma cat2cat_or_CAT_uppercased.hfst.foma \
			cat2CAT_uppercased.hfst.foma cat2dog.hfst.foma \
			cat+dog.hfst.foma catdog.hfst.foma cat_or_dog.hfst.foma \
			cat.hfst.foma CAT_uppercased2cat.hfst.foma \
			CAT_uppercased.hfst.foma cat_weight_final.hfst.foma \
			cat_weight_initial.hfst.foma cat_weight_middle.hfst.foma \
			dog2cat.hfst.foma dog.hfst.foma infinitely_ambiguous.hfst.foma \
			non_minimal.hfst.foma rule1.hfst.foma rule2.hfst.foma \
			summarize_test1.hfst.foma summarize_test2.hfst.foma tac.hfst.foma\
			unknown2a.hfst.foma identity.hfst.foma abid.hfst.foma unk2unk.hfst.foma unk_or_id_star.hfst.foma \
			a2b.hfst.foma a2b_complement.hfst.foma a2b_input_projection_complement.hfst.foma \
			empty.hfst.foma \
			walk_or_dog.hfst.foma
endif

check_DATA=$(DEFAULT_CHECKS) $(SFST_CHECKS) $(OFST_CHECKS) $(FOMA_CHECKS) $(OL_CHECKS)

FST_TXTS=0to3cats.txt 2to4cats.txt 4cats.txt 4toINFcats.txt \
		 cat2cat_or_CAT_uppercased.txt cat2CAT_uppercased.txt \
		 cat2epsilon.txt cat2dog.txt \
		 cat+dog.txt catdog.txt cat_or_dog.txt cat.txt CAT_uppercased2cat.txt \
		 CAT_uppercased.txt cat_weight_final.txt cat_weight_initial.txt \
		 cat_weight_middle.txt dog2cat.txt dog.txt\
		 epsilon.txt epsilon2cat.txt infinitely_ambiguous.txt \
		 non_minimal.txt rule1.txt rule2.txt summarize_test1.txt \
		 summarize_test2.txt tac.txt proc-caps.txt \
		 unknown-star.txt identity-star.txt unknown2a.txt identity.txt\
		 heavycat.txt abid.txt unk2unk.txt cat2dog_0.3.txt cat2dog_0.5.txt empty.txt unk_or_id_star.txt \
		 a2b.txt a2b_complement.txt a2b_input_projection_complement.txt \
		 walk_or_dog.txt
FST_STRINGS=cat.strings proc-caps-in.strings proc-caps-out1.strings \
		 proc-caps-out2.strings proc-caps-out3.strings proc-caps-out4.strings \
		 proc-cat-out.strings proc-cat-weighted-out.strings \
		 utf-8.strings latin-1.strings \
		 cat2dog.strings heavycat.strings
FST_PAIRS=cat2dog.pairs
FST_PAIRSTRINGS=cat2dog.pairstring
FST_SPACESTRINGS=cat2dog.spaces
SUBSTITUTE_TXTS=cat2dog.substitute
XRE_TXTS=cats_and_dogs.xre cats_and_dogs_semicolon.xre
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
		  xre.numeric-star.lexc xre.sharp.lexc xre.star-plus-optional.lexc
#XFST_TXTS=cat-regex.xfst
ALL_SRCS=$(FST_TXTS) $(FST_STRINGS) $(FST_PAIRS) $(FST_PAIRSTRINGS) \
		$(FST_SPACESTRINGS) $(SUBSTITUTE_TXTS) $(XRE_TEXTS)

# this compilation is uesd by check target before running the tests
%.hfst: %.txt
	${top_builddir}/tools/src/hfst-txt2fst -e '@0@' $< > $@

%.hfst.sfst: %.txt
	${top_builddir}/tools/src/hfst-txt2fst -f sfst -e '@0@' $< > $@

%.hfst.ofst: %.txt
	${top_builddir}/tools/src/hfst-txt2fst -f openfst-tropical -e '@0@' $< > $@

%.hfst.foma: %.txt
	${top_builddir}/tools/src/hfst-txt2fst -f foma -e '@0@' $< > $@

%.hfst.ol: %.txt
	${top_builddir}/tools/src/hfst-txt2fst -e '@0@' -i $< | ${top_builddir}/tools/src/hfst-fst2fst -w -o $@

%.gen.hfst.ol: %.txt
	${top_builddir}/tools/src/hfst-txt2fst -e '@0@' -i $< | ${top_builddir}/tools/src/hfst-invert | ${top_builddir}/tools/src/hfst-fst2fst -w -o $@

EXTRA_FILES=cat2dog.substitute cat.strings cat.txt cats_and_dogs.xre utf-8.strings latin-1.strings c_a_t.strings cat.lexc

# files needed for test programs
EXTRA_DIST=empty-file $(FST_TXTS) $(FST_STRINGS) $(FST_PAIRS) $(FST_PAIRSTRINGS) $(FST_SPACESTRINGS) $(SUBSTITUTE_TXTS) $(XRE_TXTS) $(XFST_TXTS) $(TESTS) $(EXTRA_FILES) $(LEXC_TXTS)

clean-local:
	-rm -f $(check_DATA)
