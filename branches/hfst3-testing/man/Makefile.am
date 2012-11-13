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
			   hfst-concatenate.1 hfst-determinize.1 \
			   hfst-disjunct.1 \
			   hfst-fst2fst.1 hfst-fst2strings.1 \
			   hfst-fst2txt.1 \
			   hfst-head.1 hfst-conjunct.1 \
			   hfst-invert.1 hfst-minimize.1 hfst-project.1 \
			   hfst-push-weights.1 hfst-regexp2fst.1 \
			   hfst-remove-epsilons.1 hfst-repeat.1 \
			   hfst-reverse.1 hfst-split.1 hfst-strings2fst.1 \
			   hfst-subtract.1 hfst-summarize.1 hfst-tail.1 hfst-txt2fst.1 \
			   hfst-calculate.1 hfst-compose-intersect.1 \
			   hfst-lookup.1 \
			   hfst-substitute.1 hfst-format.1 hfst-name.1
broken=hfst-xfst.1 hfst-foma-wrapper.1 hfst-apertium-proc.1 hfst-lexc.1
aliases= hfst-determinise.1 hfst-expand.1 hfst-intersect.1 hfst-minimise.1 \
	 hfst-minus.1 hfst-sfstpl2fst.1 hfst-summarise.1 hfst-union.1
dist_man1_MANS=$(bootstrappable) $(aliases) $(broken)
LN_S=ln -s -f

hfst-determinise.1:
	$(LN_S) hfst-determinize.1 hfst-determinise.1 

hfst-expand.1:
	$(LN_S) hfst-fst2strings.1 hfst-expand.1 

hfst-intersect.1:
	$(LN_S) hfst-conjunct.1 hfst-intersect.1 

hfst-minimise.1:
	$(LN_S) hfst-minimize.1 hfst-minimise.1 

hfst-minus.1:
	$(LN_S) hfst-subtract.1 hfst-minus.1 

hfst-sfstpl2fst.1:
	$(LN_S) hfst-calculate.1 hfst-sfstpl2fst.1 

hfst-summarise.1: 
	$(LN_S) hfst-summarize.1 hfst-summarise.1 

hfst-union.1:
	$(LN_S) hfst-disjunct.1 hfst-union.1

man:
	make bootstrap && make aliases

bootstrap: 
	for f in $(bootstrappable) ; do \
	  if [ "$$f" != "hfst-lookup.1" ]; then \
		N=`echo $$f | sed 's/.1/ \-\-help/' | sed 's/hfst/..\/tools\/src\/hfst/' | sh | sed -n 2p`; \
		help2man \
		-n="$$N" \
		-s 1 -o $$f -N -S HFST `echo $$f | sed 's/.1//' | sed 's/hfst/..\/tools\/src\/hfst/'`; \
	  fi; \
	done;


aliases:
	for f in $(aliases) ; do \
		make $$f; \
	done; 

