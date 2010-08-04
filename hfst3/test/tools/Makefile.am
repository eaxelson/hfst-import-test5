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
TESTS=hfst-check-version.sh hfst-check-help.sh empty-input.sh\
	  permutate-file-params.sh \
	  compare-functionality.sh concatenate-functionality.sh \
	  conjunct-functionality.sh disjunct-functionality.sh \
	  subtract-functionality.sh \
	  determinize-functionality.sh minimize-functionality.sh \
	  remove-epsilons-functionality.sh \
	  invert-functionality.sh project-functionality.sh repeat-functionality.sh \
	  reverse-functionality.sh \
	  strings2fst-functionality.sh
check_DATA=0to3cats.hfst 2to4cats.hfst 4cats.hfst 4toINFcats.hfst \
		   cat2cat_or_CAT_uppercased.hfst cat2CAT_uppercased.hfst cat2dog.hfst \
		   cat+dog.hfst catdog.hfst cat_or_dog.hfst cat.hfst \
		   CAT_uppercased2cat.hfst CAT_uppercased.hfst cat_weight_final.hfst \
		   cat_weight_initial.hfst cat_weight_middle.hfst dog2cat.hfst \
		   dog.hfst infinitely_ambiguous.hfst non_minimal.hfst rule1.hfst \
		   rule2.hfst summarize_test1.hfst summarize_test2.hfst tac.hfst 

FST_SRCS=0to3cats.txt 2to4cats.txt 4cats.txt 4toINFcats.txt \
		 cat2cat_or_CAT_uppercased.txt cat2CAT_uppercased.txt cat2dog.txt \
		 cat+dog.txt catdog.txt cat_or_dog.txt cat.txt CAT_uppercased2cat.txt \
		 CAT_uppercased.txt cat_weight_final.txt cat_weight_initial.txt \
		 cat_weight_middle.txt dog2cat.txt dog.txt infinitely_ambiguous.txt \
		 non_minimal.txt rule1.txt rule2.txt summarize_test1.txt \
		 summarize_test2.txt tac.txt 

%.hfst: %.txt
	${top_builddir}/tools/src/hfst-txt2fst -e '@0@' < $< > $@

# files needed for test programs
EXTRA_DIST=empty-file $(FST_SRCS)
