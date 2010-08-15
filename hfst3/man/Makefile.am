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


bootstrappable=hfst-compare.1 hfst-compose.1 \
			   hfst-concatenate.1 hfst-determinize.1 hfst-diff-test.1 \
			   hfst-disjunct.1 \
			   hfst-fst2fst.1 hfst-fst2pairstrings.1 hfst-fst2strings.1 \
			   hfst-fst2txt.1 \
			   hfst-head.1 hfst-intersect.1 \
			   hfst-invert.1 hfst-minimize.1 hfst-project.1 \
			   hfst-push-weights.1 hfst-regexp2fst.1 \
			   hfst-remove-epsilons.1 hfst-repeat.1 \
			   hfst-reverse.1 hfst-split.1 hfst-strings2fst.1 \
			   hfst-subtract.1 hfst-summarize.1 hfst-tail.1 hfst-txt2fst.1 \
			   hfst-unweighted2weighted.1 hfst-weighted2unweighted.1
broken=hfst-calculate.1 hfst-compose-intersect.1 hfst-pair-test.1 hfst-proc.1
dist_man1_MANS=$(bootstrappable) $(broken)

bootstrap: 
	for f in $(bootstrappable) ; do \
		help2man -s 1 $< -o $@ -N -S HFST ; \
	done
