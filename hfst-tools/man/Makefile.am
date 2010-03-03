## Process this file with automake to produce Makefile.in
## Author: Tommi A Pirinen <tommi.pirinen@helsinki.fi>
## Licence: GPL v3 (not newer)

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
broken=hfst-calculate.1 hfst-compose-intersect.1 hfst-pair-test.1
dist_man1_MANS=$(bootstrappable) $(broken)
all-local:
	true

bootstrap: 
	for f in $(bootstrappable) ; do \
		help2man -s 1 $< -o $@ -N -S HFST ; \
	done
