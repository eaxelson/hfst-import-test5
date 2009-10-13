## Process this file with automake to produce Makefile.in
## Author: HFST Team <hfst-bugs@helsinki.fi>
## Licence: GPL v3 (not newer)

SUBDIRS=. binary-tools
TEXTFSTSRCS=aA.txt catdog.txt cat.txt CAT.txt cat_or_dog.txt cCaAtT.txt ccCaAtT.txt \
CcAaTt_.txt dog.txt tT.txt cat+dog.txt cat2dog.txt tac.txt cat_2_5.txt cat_4_INF.txt cat_0_3.txt \
4cats.txt summarize_test1.txt summarize_test2.txt non_minimal.txt dog2cat.txt cat_weight_middle.txt \
cat_weight_initial.txt cat_weight_final.txt \
infinitely_ambiguous.txt

OTHERSRCS=4symbols animals animals_and_frequencies animals_pairstring \
animals_spaces calculate_input calculate_test cat_alphabet cat_dog \
cat_dog_alphabet cat_dog_pair_strings cat_dog_pair_strings_spaces \
cat_dog_strings cat_dog_strings_spaces cat.number cat_pairs cat_pairs_spaces \
cat_sets cat_sets_spaces regexp1 regexp2 summarized1 summarized2 symbols \
cat_symbols dog_symbols cat_symbols_and_epsilon dog_symbols_and_epsilon \
cat2dog.input cat2dog.output infinitely_ambiguous_inputs cat.hfst.names

SRCS= $(TEXTFSTSRCS) $(OTHERSRCS)

CRASH_TST=./hfst-crash-test.sh

EXTRA_DIST=$(SRCS) $(CRASH_TST) separator


# these tools take two transducers as input and write one transducer as output
BINARY_TOOLS= \
	hfst-compose hfst-concatenate hfst-conjunct hfst-disjunct

# these tools take one transducer as input and write one transducer as output
# both weighted and unweighted transducers are valid input
UNARY_TOOLS= \
	hfst-fst2fst hfst-determinize \
	hfst-head hfst-invert hfst-minimize hfst-project \
	hfst-push-weights hfst-remove-epsilons hfst-repeat \
	hfst-reverse hfst-symbols hfst-tail
# unary tools that have specific options and other tools
OTHER_TOOLS= \
	hfst-project hfst-push-weights hfst-repeat hfst-head hfst-tail \
	hfst-split hfst-compare hfst-summarize hfst-fst2fst \
	hfst-fst2txt hfst-txt2fst hfst-fst2strings hfst-strings2fst \
	hfst-lookup # hfst-regexp2fst 	hfst-unweighted2weighted hfst-weighted2unweighted


# minimize, determinize, remove-epsilons
# cat test_transducers.hfst | $(top_builddir)/src/hfst-program -s | $(top_builddir)/src/hfst-equivalent -e -1 - -2 test_transducers.hfst  
# cat test_transducers.hwfst | $(top_builddir)/src/hfst-program -s | $(top_builddir)/src/hfst-equivalent -e -1 - -2 test_transducers.hwfst  

check-local: functionality commandline

# for BINARY TOOLS
%-binary-commandline-test: cat.hfst dog.hfst symbols

	$(top_builddir)/src/$* -1 cat.hfst -2 dog.hfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/$* --input1 cat.hfst --input2 dog.hfst --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.hfst | $(top_builddir)/src/$* -s dog.hfst 2>1 > /dev/null # stdin and free parameters
	cat cat.hfst | $(top_builddir)/src/$* -s -2 dog.hfst 2>1 > /dev/null # stdin parameter
	cat dog.hfst | $(top_builddir)/src/$* -s -1 cat.hfst 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/$* cat.hfst dog.hfst -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/$* cat.hfst dog.hfst --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null
	$(top_builddir)/src/$* cat.hfst dog.hfst -n 2>1 > /dev/null
	$(top_builddir)/src/$* cat.hfst dog.hfst --number 2>1 > /dev/null

	$(top_builddir)/src/$* -1 cat.hwfst -2 dog.hwfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/$* --input1 cat.hwfst --input2 dog.hwfst --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.hwfst | $(top_builddir)/src/$* -s dog.hwfst 2>1 > /dev/null # stdin and free parameters
	cat cat.hwfst | $(top_builddir)/src/$* -s -2 dog.hwfst 2>1 > /dev/null # stdin parameter
	cat dog.hwfst | $(top_builddir)/src/$* -s -1 cat.hwfst 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/$* cat.hwfst dog.hwfst -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/$* cat.hwfst dog.hwfst --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null
	$(top_builddir)/src/$* cat.hwfst dog.hwfst -n 2>1 > /dev/null
	$(top_builddir)/src/$* cat.hwfst dog.hwfst --number 2>1 > /dev/null

	$(top_builddir)/src/$* -h 2>1 > /dev/null
	$(top_builddir)/src/$* --help 2>1 > /dev/null
	$(top_builddir)/src/$* -V 2>1 > /dev/null
	$(top_builddir)/src/$* --version 2>1 > /dev/null


# for UNARY_TOOLS
%-unary-commandline-test: cat.hfst cat.hwfst dog.hfst dog.hwfst symbols

	$(top_builddir)/src/$* -i cat.hfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/$* --input cat.hfst --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.hfst dog.hfst | $(top_builddir)/src/$* -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/$* -s cat.hfst 2>1 > /dev/null # free parameter
	$(top_builddir)/src/$* cat.hfst -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/$* cat.hfst --read-symbols symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null

	$(top_builddir)/src/$* -i cat.hwfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/$* --input cat.hwfst --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.hwfst dog.hwfst | $(top_builddir)/src/$* -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/$* -s cat.hwfst 2>1 > /dev/null # free parameter
	$(top_builddir)/src/$* cat.hwfst -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/$* cat.hwfst --read-symbols symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null

	$(top_builddir)/src/$* -h 2>1 > /dev/null
	$(top_builddir)/src/$* --help 2>1 > /dev/null
	$(top_builddir)/src/$* -V 2>1 > /dev/null
	$(top_builddir)/src/$* --version 2>1 > /dev/null


# test the rest of the arguments of UNARY_TOOLS
hfst-project-commandline-test: cat.hfst cat.hwfst
	for i in input output upper lower analysis generation; do \
	$(top_builddir)/src/hfst-project -q -s -o /dev/null -i cat.hfst -p $$i; \
	$(top_builddir)/src/hfst-project -q -s -o /dev/null -i cat.hfst --project $$i; \
	$(top_builddir)/src/hfst-project -q -s -o /dev/null -i cat.hwfst -p $$i; \
	$(top_builddir)/src/hfst-project -q -s -o /dev/null -i cat.hwfst --project $$i; \
	done

hfst-lookup-commandline-test: cat.hfst cat.hwfst
	$(top_builddir)/src/hfst-lookup -q -s -o /dev/null -i cat.hfst -I cat2dog.input 

hfst-push-weights-commandline-test: cat.hfst
	for i in start initial end final; do \
	$(top_builddir)/src/hfst-push-weights -q -s -o /dev/null -i cat.hfst -p $$i; \
	$(top_builddir)/src/hfst-push-weights -q -s -o /dev/null -i cat.hfst --push $$i; \
	$(top_builddir)/src/hfst-push-weights -q -s -o /dev/null -i cat.hwfst -p $$i; \
	$(top_builddir)/src/hfst-push-weights -q -s -o /dev/null -i cat.hwfst --push $$i; \
	done

hfst-repeat-commandline-test: cat.hfst cat.hwfst
	for i in cat.hfst cat.hwfst; do \
	$(top_builddir)/src/hfst-repeat -q -s -o /dev/null -i $$i -f 2 -t 5; \
	$(top_builddir)/src/hfst-repeat -q -s -o /dev/null -i $$i -f 4; \
	$(top_builddir)/src/hfst-repeat -q -s -o /dev/null -i $$i -t 3; \
	$(top_builddir)/src/hfst-repeat -q -s -o /dev/null -i $$i --from 2 --to 5; \
	$(top_builddir)/src/hfst-repeat -q -s -o /dev/null -i $$i --from 4; \
	$(top_builddir)/src/hfst-repeat -q -s -o /dev/null -i $$i --to 3; \
	done

hfst-head-commandline-test: cat.hfst cat.hwfst
	$(top_builddir)/src/hfst-head cat.hfst -o /dev/null -q -s -n 2
	$(top_builddir)/src/hfst-head cat.hfst -o /dev/null -q -s --n-first 2
	$(top_builddir)/src/hfst-head cat.hwfst -o /dev/null -q -s -n 2
	$(top_builddir)/src/hfst-head cat.hwfst -o /dev/null -q -s --n-first 2

hfst-tail-commandline-test: cat.hfst cat.hwfst
	$(top_builddir)/src/hfst-tail cat.hfst -o /dev/null -q -s -n 2
	$(top_builddir)/src/hfst-tail cat.hfst -o /dev/null -q -s --n-last 2
	$(top_builddir)/src/hfst-tail cat.hwfst -o /dev/null -q -s -n 2
	$(top_builddir)/src/hfst-tail cat.hwfst -o /dev/null -q -s --n-last 2


# test rest of the programs
hfst-calculate-commandline-test: symbols calculate_input

	$(top_builddir)/src/hfst-calculate -i $(srcdir)/calculate_input -o /dev/null -s -q -v -C -I  2>1 > /dev/null
	$(top_builddir)/src/hfst-calculate --input $(srcdir)/calculate_input --output /dev/null --silent --quiet --verbose --compatible --invert  2>1 > /dev/null
	cat $(srcdir)/calculate_input | $(top_builddir)/src/hfst-calculate -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-calculate -s $(srcdir)/calculate_input 2>1 > /dev/null # free parameter
	$(top_builddir)/src/hfst-calculate $(srcdir)/calculate_input -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/hfst-calculate $(srcdir)/calculate_input --read-symbols symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null

	$(top_builddir)/src/hfst-calculate -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-calculate --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-calculate -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-calculate --version 2>1 > /dev/null


hwfst-calculate-commandline-test: symbols calculate_input

	$(top_builddir)/src/hwfst-calculate -i $(srcdir)/calculate_input -o /dev/null -s -q -v -C -I 2>1 > /dev/null
	$(top_builddir)/src/hwfst-calculate --input $(srcdir)/calculate_input --output /dev/null --silent --quiet --verbose --compatible --invert 2>1 > /dev/null
	cat $(srcdir)/calculate_input | $(top_builddir)/src/hwfst-calculate -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hwfst-calculate -s $(srcdir)/calculate_input 2>1 > /dev/null # free parameter
	$(top_builddir)/src/hwfst-calculate $(srcdir)/calculate_input -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/hwfst-calculate $(srcdir)/calculate_input --read-symbols symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null

	$(top_builddir)/src/hwfst-calculate -h 2>1 > /dev/null
	$(top_builddir)/src/hwfst-calculate --help 2>1 > /dev/null
	$(top_builddir)/src/hwfst-calculate -V 2>1 > /dev/null
	$(top_builddir)/src/hwfst-calculate --version 2>1 > /dev/null


hfst-split-commandline-test: 

	cat cat.hfst dog.hfst > cat_dog.hfst	
	$(top_builddir)/src/hfst-split -i cat_dog.hfst -p 'TMP' -e '.hfst' -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-split --input cat_dog.hfst --prefix 'TMP' --extension '.hfst' --silent --quiet --verbose 2>1 > /dev/null
	cat cat_dog.hfst | $(top_builddir)/src/hfst-split -p 'TMP' -e '.hfst' -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-split -s cat_dog.hfst -p 'TMP' -e '.hfst' 2>1 > /dev/null # free parameter
	$(top_builddir)/src/hfst-split cat_dog.hfst -p 'TMP' -e '.hfst' -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/hfst-split cat_dog.hfst -p 'TMP' -e '.hfst' --read-symbols symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null
	rm -f cat_dog.hfst

	cat cat.hwfst dog.hwfst > cat_dog.hwfst	
	$(top_builddir)/src/hfst-split -i cat_dog.hwfst -p 'TMP' -e '.hwfst' -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-split --input cat_dog.hwfst --prefix 'TMP' --extension '.hwfst' --silent --quiet --verbose 2>1 > /dev/null
	cat cat_dog.hwfst | $(top_builddir)/src/hfst-split -p 'TMP' -e '.hwfst' -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-split -s cat_dog.hwfst -p 'TMP' -e '.hwfst' 2>1 > /dev/null # free parameter
	$(top_builddir)/src/hfst-split cat_dog.hwfst -p 'TMP' -e '.hwfst' -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/hfst-split cat_dog.hwfst -p 'TMP' -e '.hwfst' --read-symbols symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null
	rm -f cat_dog.hwfst

	$(top_builddir)/src/hfst-split -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-split --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-split -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-split --version 2>1 > /dev/null

	rm -f TMP1.hfst TMP1.hwfst TMP2.hfst TMP2.hwfst cat_dog.hfst cat_dog.hwfst


hfst-compare-commandline-test: cat.hfst dog.hfst

	$(top_builddir)/src/hfst-compare -1 cat.hfst -2 cat.hfst -o /dev/null -s -q -v -e 2>1 > /dev/null
	$(top_builddir)/src/hfst-compare --input1 dog.hfst --input2 dog.hfst --output /dev/null --silent --quiet --verbose --error-on-mismatch 2>1 > /dev/null
	cat cat.hfst | $(top_builddir)/src/hfst-compare -s dog.hfst 2>1 > /dev/null # stdin and free parameters
	cat cat.hfst | $(top_builddir)/src/hfst-compare -s -2 dog.hfst 2>1 > /dev/null # stdin parameter
	cat dog.hfst | $(top_builddir)/src/hfst-compare -s -1 cat.hfst 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-compare cat.hfst dog.hfst -n 2>1 > /dev/null
	$(top_builddir)/src/hfst-compare cat.hfst dog.hfst --number 2>1 > /dev/null

	$(top_builddir)/src/hfst-compare -1 cat.hwfst -2 dog.hwfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-compare --input1 cat.hwfst --input2 dog.hwfst --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.hwfst | $(top_builddir)/src/hfst-compare -s dog.hwfst 2>1 > /dev/null # stdin and free parameters
	cat cat.hwfst | $(top_builddir)/src/hfst-compare -s -2 dog.hwfst 2>1 > /dev/null # stdin parameter
	cat dog.hwfst | $(top_builddir)/src/hfst-compare -s -1 cat.hwfst 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-compare cat.hwfst dog.hwfst -n 2>1 > /dev/null
	$(top_builddir)/src/hfst-compare cat.hwfst dog.hwfst --number 2>1 > /dev/null

	$(top_builddir)/src/hfst-compare -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-compare --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-compare -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-compare --version 2>1 > /dev/null


hfst-summarize-commandline-test: cat.hfst cat.hwfst dog.hfst dog.hwfst

	$(top_builddir)/src/hfst-summarize -i cat.hfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-summarize --input cat.hfst --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.hfst dog.hfst | $(top_builddir)/src/hfst-summarize -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-summarize -s cat.hfst 2>1 > /dev/null # free parameter

	$(top_builddir)/src/hfst-summarize -i cat.hwfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-summarize --input cat.hwfst --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.hwfst dog.hwfst | $(top_builddir)/src/hfst-summarize -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-summarize -s cat.hwfst 2>1 > /dev/null # free parameter

	$(top_builddir)/src/hfst-summarize -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-summarize --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-summarize -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-summarize --version 2>1 > /dev/null


hfst-fst2fst-commandline-test: cat.hfst cat.hwfst dog.hfst dog.hwfst symbols

	$(top_builddir)/src/hfst-fst2fst -i cat.hfst -o /dev/null -s -q -v -f sfst 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst --input cat.hfst --output /dev/null --silent --quiet --verbose --format openfst 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -i cat.hfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst --input cat.hfst --output /dev/null --silent --quiet --verbose --format sfst 2>1 > /dev/null

	$(top_builddir)/src/hfst-fst2fst -i cat.hwfst -o /dev/null -s -q -v -f openfst 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst --input cat.hwfst --output /dev/null --silent --quiet --verbose --format sfst 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -i cat.hwfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst --input cat.hwfst --output /dev/null --silent --quiet --verbose --format openfst 2>1 > /dev/null


	$(top_builddir)/src/hfst-fst2fst -w -i cat.hfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -w --input cat.hfst --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.hfst dog.hfst | $(top_builddir)/src/hfst-fst2fst -w -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-fst2fst -w -s cat.hfst 2>1 > /dev/null # free parameter
	$(top_builddir)/src/hfst-fst2fst -w cat.hfst -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -w cat.hfst --read-symbols symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null

	$(top_builddir)/src/hfst-fst2fst -w -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -w --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -w -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -w --version 2>1 > /dev/null


	$(top_builddir)/src/hfst-fst2fst -u -i cat.hwfst -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -u --input cat.hwfst --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.hwfst dog.hwfst | $(top_builddir)/src/hfst-fst2fst -u -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-fst2fst -u -s cat.hwfst 2>1 > /dev/null # free parameter
	$(top_builddir)/src/hfst-fst2fst -u cat.hwfst -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -u cat.hwfst --read-symbols symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null

	$(top_builddir)/src/hfst-fst2fst -u -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -u --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -u -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2fst -u --version 2>1 > /dev/null


hfst-fst2fst-functionality-test: 2cat.hfst 2cat.hwfst cat.hfst cat.hwfst

# HFST -> HWFST -> HFST
	cat 2cat.hfst | $(top_builddir)/src/hfst-fst2fst -w -s | \
	$(top_builddir)/src/hfst-fst2fst -u -s | $(top_builddir)/src/hfst-compare -e 2cat.hfst  # 2>1 > /dev/null
# HWFST -> HFST -> HWFST
	cat 2cat.hwfst | $(top_builddir)/src/hfst-fst2fst -u -s | \
	$(top_builddir)/src/hfst-fst2fst -w -s | $(top_builddir)/src/hfst-compare -e 2cat.hwfst  # 2>1 > /dev/null

	echo "0061" > SFST; echo "0050" > HFST; echo "00d6" > OFST; echo "0041" > HWFST;
# HFST -> SFST
	cat cat.hfst | $(top_builddir)/src/hfst-fst2fst -f sfst > cat.sfst
	cat cat.sfst | hexdump -n 1 | tr ' ' '\n' | egrep "^[0-9a-f]{4}$$" > TMP; diff TMP SFST
# HWFST -> OFST
	cat cat.hwfst | $(top_builddir)/src/hfst-fst2fst -f openfst > cat.ofst
	cat cat.ofst | hexdump -n 1 | tr ' ' '\n' | egrep "^[0-9a-f]{4}$$" > TMP; diff TMP OFST
# SFST -> HFST
	cat cat.sfst | $(top_builddir)/src/hfst-fst2fst -u | hexdump -n 1 | tr ' ' '\n' | egrep "^[0-9a-f]{4}$$" > TMP; diff TMP HFST
# OFST -> HWFST
	cat cat.ofst | $(top_builddir)/src/hfst-fst2fst -w | hexdump -n 1 | tr ' ' '\n' | egrep "^[0-9a-f]{4}$$" > TMP; diff TMP HWFST
# HFST -> OFST
	cat cat.hfst | $(top_builddir)/src/hfst-fst2fst -f openfst | hexdump -n 1 | tr ' ' '\n' | egrep "^[0-9a-f]{4}$$" > TMP; diff TMP OFST
# HWFST -> SFST
	cat cat.hwfst | $(top_builddir)/src/hfst-fst2fst -f sfst | hexdump -n 1 | tr ' ' '\n' | egrep "^[0-9a-f]{4}$$" > TMP; diff TMP SFST
# OFST -> HFST
	cat cat.ofst | $(top_builddir)/src/hfst-fst2fst -u | hexdump -n 1 | tr ' ' '\n' | egrep "^[0-9a-f]{4}$$" > TMP; diff TMP HFST
# SFST -> HWFST
	cat cat.sfst | $(top_builddir)/src/hfst-fst2fst -w | hexdump -n 1 | tr ' ' '\n' | egrep "^[0-9a-f]{4}$$" > TMP; diff TMP HWFST

	rm TMP SFST OFST HFST HWFST cat.sfst cat.ofst;


hfst-fst2strings-commandline-test: cat.hfst cat.hwfst symbols

	for i in cat.hfst cat.hwfst; do \
	$(top_builddir)/src/hfst-fst2strings -i $$i -o /dev/null -s -q -v 2>1 > /dev/null; \
	$(top_builddir)/src/hfst-fst2strings --input $$i --output /dev/null --silent --quiet --verbose 2>1 > /dev/null; \
	cat $$i | $(top_builddir)/src/hfst-fst2strings -s 2>1 > /dev/null; \
	$(top_builddir)/src/hfst-fst2strings -s $$i 2>1 > /dev/null; \
	$(top_builddir)/src/hfst-fst2strings $$i -R symbols 2>1 > /dev/null; \
	$(top_builddir)/src/hfst-fst2strings $$i --read-symbols symbols 2>1 > /dev/null; \
	\
	$(top_builddir)/src/hfst-fst2strings -q -s -S -p -u -r 2 -n 3 -w -e -i $$i -o /dev/null -R symbols; \
	$(top_builddir)/src/hfst-fst2strings --quiet --silent --print-spaces --pairstring --unique --random --nbest 3 --print-epsilons --print-weights --input $$i --output /dev/null --read-symbols symbols; \
	done

	$(top_builddir)/src/hfst-fst2strings -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2strings --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2strings -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2strings --version 2>1 > /dev/null


hfst-fst2txt-commandline-test: cat.hfst cat.hwfst symbols

	for i in cat.hfst cat.hwfst; do \
	$(top_builddir)/src/hfst-fst2txt -i $$i -o /dev/null -s -q -v 2>1 > /dev/null; \
	$(top_builddir)/src/hfst-fst2txt --input $$i --output /dev/null --silent --quiet --verbose 2>1 > /dev/null; \
	cat $$i | $(top_builddir)/src/hfst-fst2txt -s 2>1 > /dev/null; \
	$(top_builddir)/src/hfst-fst2txt -s $$i 2>1 > /dev/null; \
	$(top_builddir)/src/hfst-fst2txt $$i -R symbols 2>1 > /dev/null; \
	$(top_builddir)/src/hfst-fst2txt $$i --read-symbols symbols 2>1 > /dev/null; \
	\
	$(top_builddir)/src/hfst-fst2txt -q -s -n -w -i $$i -o /dev/null -R symbols; \
	$(top_builddir)/src/hfst-fst2txt --quiet --silent --number --print-weights --input $$i --output /dev/null --read-symbols symbols; \
	done

	$(top_builddir)/src/hfst-fst2txt -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2txt --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2txt -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-fst2txt --version 2>1 > /dev/null


hfst-strings2fst-commandline-test: animals symbols

	$(top_builddir)/src/hfst-strings2fst -i animals -R symbols -o /dev/null -s -q -v 2>1 > /dev/null
	$(top_builddir)/src/hfst-strings2fst --input animals --read-symbols symbols --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat animals | $(top_builddir)/src/hfst-strings2fst -R symbols -s 2>1 > /dev/null
	$(top_builddir)/src/hfst-strings2fst -s animals -R symbols 2>1 > /dev/null
	$(top_builddir)/src/hfst-strings2fst animals -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/hfst-strings2fst animals --read-symbols symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null

	$(top_builddir)/src/hfst-strings2fst -i animals_pairstring -R symbols -o /dev/null -s -q -v --pairstring -j --sum --norm --log 2>1 > /dev/null
	$(top_builddir)/src/hfst-strings2fst -i animals_and_frequencies -R symbols -o /dev/null -s -q -v --weighted --disjunct-strings --sum --norm --log 2>1 > /dev/null

	$(top_builddir)/src/hfst-strings2fst -i animals_spaces -S -e '<>' -o /dev/null -s -q -v --weighted 2>1 > /dev/null

	$(top_builddir)/src/hfst-strings2fst -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-strings2fst --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-strings2fst -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-strings2fst --version 2>1 > /dev/null


hfst-txt2fst-commandline-test: cat.txt cat.number symbols

	$(top_builddir)/src/hfst-txt2fst -i cat.number -n -o /dev/null -s -q -v 2>1 > /dev/null # in number format
	$(top_builddir)/src/hfst-txt2fst --input cat.number --number --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
	cat cat.number | $(top_builddir)/src/hfst-txt2fst -n -s 2>1 > /dev/null
	$(top_builddir)/src/hfst-txt2fst -s cat.number -n 2>1 > /dev/null
	$(top_builddir)/src/hfst-txt2fst cat.txt -R symbols -w -D -W TMP 2>1 > /dev/null # in text format
	$(top_builddir)/src/hfst-txt2fst cat.txt --read-symbols symbols --weighted  --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null
	$(top_builddir)/src/hfst-txt2fst cat.txt -e '<>' 2>1 > /dev/null # no symbol table given
	$(top_builddir)/src/hfst-txt2fst cat.txt --epsilon '<>' 2>1 > /dev/null

	$(top_builddir)/src/hfst-txt2fst -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-txt2fst --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-txt2fst -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-txt2fst --version 2>1 > /dev/null

#hfst-regexp2fst-commandline-test: regexp1 regexp2 4symbols

#	$(top_builddir)/src/hfst-regexp2fst -i regexp1 -R 4symbols -o /dev/null -s -q -v 2>1 > /dev/null
#	$(top_builddir)/src/hfst-regexp2fst --input regexp1 --read-symbols 4symbols --output /dev/null --silent --quiet --verbose 2>1 > /dev/null
#	cat regexp1 | $(top_builddir)/src/hfst-regexp2fst -s 2>1 > /dev/null
#	$(top_builddir)/src/hfst-regexp2fst -s -e 'a:b ?' 2>1 > /dev/null
#	$(top_builddir)/src/hfst-regexp2fst -s regexp1 -R 4symbols 2>1 > /dev/null
#	$(top_builddir)/src/hfst-regexp2fst regexp1 -R symbols -D -W TMP 2>1 > /dev/null
#	$(top_builddir)/src/hfst-regexp2fst regexp1 --read-symbols 4symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null

#	cat regexp1 regexp2 | $(top_builddir)/src/hfst-regexp2fst -o /dev/null -s -q -v -w 0.5 -j 2>1 > /dev/null
#	cat regexp1 regexp2 | $(top_builddir)/src/hfst-regexp2fst -o /dev/null -s -q -v --weighted 0.5 --disjunct-expressions 2>1 > /dev/null
#	cat regexp1 regexp2 | $(top_builddir)/src/hfst-regexp2fst -o /dev/null -s -q -v 2>1 > /dev/null

#	$(top_builddir)/src/hfst-regexp2fst -h 2>1 > /dev/null
#	$(top_builddir)/src/hfst-regexp2fst --help 2>1 > /dev/null
#	$(top_builddir)/src/hfst-regexp2fst -V 2>1 > /dev/null
#	$(top_builddir)/src/hfst-regexp2fst --version 2>1 > /dev/null


test: \
	fsts wfsts 2txts 2fsts 2wfsts commandline functionality \
	unary-commandline binary-commandline \
	other-commandline

fsts: 	$(TEXTFSTSRCS:%.txt=%.hfst)

wfsts: 	$(TEXTFSTSRCS:%.txt=%.hwfst)

2fsts: 	$(TEXTFSTSRCS:%.txt=2%.hfst)

2wfsts:	$(TEXTFSTSRCS:%.txt=2%.hwfst)

2txts: $(TEXTFSTSRCS:%.txt=2%.txt)


clean-local:
	rm -f $(TEXTFSTSRCS:%.txt=%.hfst)
	rm -f $(TEXTFSTSRCS:%.txt=%.hwfst)
	rm -f $(TEXTFSTSRCS:%.txt=2%.hfst)
	rm -f $(TEXTFSTSRCS:%.txt=2%.hwfst)
	rm -f TMP TMP2 1
	rm -f $(TEXTFSTSRCS:%.txt=2%.txt)
	rm -f CAT DOG
	rm -f cat2dog.temp animals.hwfst

functionality: \
	hfst-remove-epsilons-functionality-test \
	hfst-determinize-functionality-test \
	hfst-minimize-functionality-test \
	hfst-push-weights-functionality-test \
        hfst-invert-functionality-test \
        hfst-reverse-functionality-test \
	hfst-project-functionality-test \
	hfst-fst2strings-functionality-test \
	hfst-strings2fst-functionality-test \
	hfst-strings-functionality-test \
	hfst-head-functionality-test \
	hfst-tail-functionality-test \
	hfst-split-functionality-test \
	hfst-repeat-functionality-test \
	hfst-fst2txt-functionality-test \
	hfst-fst2fst-functionality-test \
	hfst-txt2fst-functionality-test \
	hfst-symbols-functionality-test \
	hfst-calculate-functionality-test \
	hwfst-calculate-functionality-test \
	hfst-summarize-functionality-test \
	hfst-pair-test-functionality-test \
	hfst-diff-test-functionality-test \
	hfst-compose-intersect-functionality-test \
	hfst-lookup-functionality-test
#	hfst-unweighted2weighted-functionality-test
#	hfst-weighted2unweighted-functionality-test
#	hfst-X-functionality-test

commandline: \
	hfst-calculate-commandline-test \
	hwfst-calculate-commandline-test \
	hfst-pair-test-commandline-test \
	hfst-diff-test-commandline-test \
        hfst-compose-intersect-commandline-test
#	hfst-X-commandline-test

unary-commandline: \
	$(UNARY_TOOLS:%=%-unary-commandline-test)

binary-commandline: \
	$(BINARY_TOOLS:%=%-binary-commandline-test)

other-commandline: \
	$(OTHER_TOOLS:%=%-commandline-test)



#crash: \
#	hfst-lookup-crash-test

# Test main functionality...
hfst-pair-test-functionality-test: cat.hfst cat_pairs_spaces cat_pairs
	$(top_builddir)/src/hfst-pair-test -p $(srcdir)/cat_pairs -s cat.hfst
	$(top_builddir)/src/hfst-pair-test -S -p $(srcdir)/cat_pairs_spaces -s cat.hfst

hfst-diff-test-functionality-test: cat.hfst cat_sets cat_sets_spaces
	$(top_builddir)/src/hfst-diff-test -r $(srcdir)/cat_sets -s cat.hfst 
	$(top_builddir)/src/hfst-diff-test -r $(srcdir)/cat_sets_spaces -S -s cat.hfst 

#hfst-invert-functionality-test: cCaAtT.hfst CcAaTt_.hfst
#	cat cCaAtT.hfst | $(top_builddir)/src/hfst-invert > CcAaTt_.hfst.1
#	$(top_builddir)/src/hfst-compare -e -q CcAaTt_.hfst CcAaTt_.hfst.1
#	rm -f CcAaTt_.hfst.1

hfst-compose-intersect-functionality-test: cat.hfst aA.hfst tT.hfst ccCaAtT.hfst
	cat cat.hfst aA.hfst tT.hfst | $(top_builddir)/src/hfst-compose-intersect -s > ccCaAtT.hfst.1
	$(top_builddir)/src/hfst-compare -1 ccCaAtT.hfst -2 ccCaAtT.hfst.1
	rm -f ccCaAtT.hfst.1

hfst-project-functionality-test: 2cat.hfst 2CAT.hfst 2cCaAtT.hfst  2cat.hwfst 2CAT.hwfst 2cCaAtT.hwfst cat.hfst
	cat 2cCaAtT.hfst | $(top_builddir)/src/hfst-project -p upper > cat.hfst.1
	$(top_builddir)/src/hfst-compare -e -1 2cat.hfst -2 cat.hfst.1 -q
	rm -f cat.hfst.1
	cat 2cCaAtT.hfst | $(top_builddir)/src/hfst-project -p input > cat.hfst.1
	$(top_builddir)/src/hfst-compare -e -1 cat.hfst -2 cat.hfst.1 -q
	rm -f cat.hfst.1
	cat 2cCaAtT.hfst | $(top_builddir)/src/hfst-project -p analysis > cat.hfst.1
	$(top_builddir)/src/hfst-compare -e -1 2cat.hfst -2 cat.hfst.1 -q
	rm -f cat.hfst.1
	cat 2cCaAtT.hfst | $(top_builddir)/src/hfst-project -p lower > CAT.hfst.1
	$(top_builddir)/src/hfst-compare -e -1 2CAT.hfst -2 CAT.hfst.1 -q
	rm -f CAT.hfst.1
	cat 2cCaAtT.hfst | $(top_builddir)/src/hfst-project -p output > CAT.hfst.1
	$(top_builddir)/src/hfst-compare -e -1 2CAT.hfst -2 CAT.hfst.1 -q
	rm -f CAT.hfst.1
	cat 2cCaAtT.hfst | $(top_builddir)/src/hfst-project -p generation > CAT.hfst.1
	$(top_builddir)/src/hfst-compare -e -1 2CAT.hfst -2 CAT.hfst.1 -q
	rm -f CAT.hfst.1
	cat 2cCaAtT.hwfst | $(top_builddir)/src/hfst-project -p upper > cat.hwfst.1
	$(top_builddir)/src/hfst-compare -e -1 2cat.hwfst -2 cat.hwfst.1 -q
	rm -f cat.hwfst.1
	cat 2cCaAtT.hwfst | $(top_builddir)/src/hfst-project -p input > cat.hwfst.1
	$(top_builddir)/src/hfst-compare -e -1 2cat.hwfst -2 cat.hwfst.1 -q
	rm -f cat.hwfst.1
	cat 2cCaAtT.hwfst | $(top_builddir)/src/hfst-project -p analysis > cat.hwfst.1
	$(top_builddir)/src/hfst-compare -e -1 2cat.hwfst -2 cat.hwfst.1 -q
	rm -f cat.hwfst.1
	cat 2cCaAtT.hwfst | $(top_builddir)/src/hfst-project -p lower > CAT.hwfst.1
	$(top_builddir)/src/hfst-compare -e -1 2CAT.hwfst -2 CAT.hwfst.1 -q
	rm -f CAT.hwfst.1
	cat 2cCaAtT.hwfst | $(top_builddir)/src/hfst-project -p output > CAT.hwfst.1
	$(top_builddir)/src/hfst-compare -e -1 2CAT.hwfst -2 CAT.hwfst.1 -q
	rm -f CAT.hwfst.1
	cat 2cCaAtT.hwfst | $(top_builddir)/src/hfst-project -p generation > CAT.hwfst.1
	$(top_builddir)/src/hfst-compare -e -1 2CAT.hwfst -2 CAT.hwfst.1 -q
	rm -f CAT.hwfst.1

hfst-reverse-functionality-test: 2cat.hfst 2cat.hwfst 2tac.hfst 2tac.hwfst
	cat 2cat.hfst | $(top_builddir)/src/hfst-reverse > tac.hfst.1
	$(top_builddir)/src/hfst-compare -e -1 2tac.hfst -2 tac.hfst.1 -q
	rm -f tac.hfst.1
	cat 2cat.hwfst | $(top_builddir)/src/hfst-reverse > tac.hwfst.1
	$(top_builddir)/src/hfst-compare -e -1 2tac.hwfst -2 tac.hwfst.1 -q
	rm -f tac.hwfst.1

hfst-invert-functionality-test: 2cat2dog.hfst 2cat2dog.hwfst 2dog2cat.hfst 2dog2cat.hwfst
	cat 2cat2dog.hfst | $(top_builddir)/src/hfst-invert > dog2cat.hfst.1
	$(top_builddir)/src/hfst-compare -e -1 2dog2cat.hfst -2 dog2cat.hfst.1 -q
	rm -f dog2cat.hfst.1
	cat 2cat2dog.hwfst | $(top_builddir)/src/hfst-invert > dog2cat.hwfst.1
	$(top_builddir)/src/hfst-compare -e -1 2dog2cat.hwfst -2 dog2cat.hwfst.1 -q
	rm -f dog2cat.hwfst.1

hfst-repeat-functionality-test: 2cat.hfst 2cat.hwfst 2cat_2_5.hfst 2cat_4_INF.hfst 2cat_0_3.hfst 2cat_2_5.hwfst 2cat_4_INF.hwfst 2cat_0_3.hwfst
	for i in hfst hwfst; do \
	$(top_builddir)/src/hfst-repeat -s -i 2cat.$$i -f 2 -t 5 | $(top_builddir)/src/hfst-compare -s -e 2cat_2_5.$$i; \
	$(top_builddir)/src/hfst-repeat -s -i 2cat.$$i -f 4 | $(top_builddir)/src/hfst-compare -s -e 2cat_4_INF.$$i; \
	$(top_builddir)/src/hfst-repeat -s -i 2cat.$$i -t 3  | $(top_builddir)/src/hfst-compare -s -e 2cat_0_3.$$i; \
	done

hfst-lookup-functionality-test: cat2dog.hfst cat2dog.hwfst cat2dog.input cat2dog.output infinitely_ambiguous.hfst infinitely_ambiguous.hwfst
	cat cat2dog.hfst | $(top_builddir)/src/hfst-lookup -I $(srcdir)/cat2dog.input > cat2dog.temp
	diff cat2dog.temp $(srcdir)/cat2dog.output
	rm -f cat2dog.temp
	cat cat2dog.hwfst | $(top_builddir)/src/hfst-lookup -I $(srcdir)/cat2dog.input > cat2dog.temp
	diff cat2dog.temp $(srcdir)/cat2dog.output
	rm -f cat2og.temp
	cat infinitely_ambiguous.hfst | $(top_builddir)/src/hfst-lookup -I infinitely_ambiguous_inputs > /dev/null 2> /dev/null
	cat infinitely_ambiguous.hwfst | $(top_builddir)/src/hfst-lookup -I infinitely_ambiguous_inputs > /dev/null 2> /dev/null

hfst-remove-epsilons-functionality-test: 2non_minimal.hfst 2non_minimal.hwfst
	cat 2non_minimal.hfst | $(top_builddir)/src/hfst-remove-epsilons -s -o TMP
	$(top_builddir)/src/hfst-compare 2non_minimal.hfst TMP 2>1 > /dev/null
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P '[0-9]+\t[0-9]+\t0\t0' -c > TMP; test TMP != 0

	cat 2non_minimal.hwfst | $(top_builddir)/src/hfst-remove-epsilons -s -o TMP
	$(top_builddir)/src/hfst-compare 2non_minimal.hwfst TMP 2>1 > /dev/null
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P '[0-9]+\t[0-9]+\t0\t0' -c > TMP; test TMP != 0


hfst-determinize-functionality-test: 2non_minimal.hfst 2non_minimal.hwfst
	cat 2non_minimal.hfst | $(top_builddir)/src/hfst-determinize -s -o TMP
	$(top_builddir)/src/hfst-compare 2non_minimal.hfst TMP 2>1 > /dev/null
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P '[0-9]+\t[0-9]+\t0\t0' -c > TMP2; test TMP2 != 0
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P 'a\ta' -c > TMP2; test TMP2 != 1

	cat 2non_minimal.hwfst | $(top_builddir)/src/hfst-determinize -s -o TMP
	$(top_builddir)/src/hfst-compare 2non_minimal.hwfst TMP 2>1 > /dev/null
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P '[0-9]+\t[0-9]+\t0\t0' -c > TMP2; test TMP2 != 0
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P 'a\ta' -c > TMP2; test TMP2 != 1


hfst-minimize-functionality-test: 2non_minimal.hfst 2non_minimal.hwfst
	cat 2non_minimal.hfst | $(top_builddir)/src/hfst-minimize -s -o TMP
	$(top_builddir)/src/hfst-compare 2non_minimal.hfst TMP 2>1 > /dev/null
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P '[0-9]+\t[0-9]+\t0\t0' -c > TMP2; test TMP2 != 0
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P 'a\ta' -c > TMP2; test TMP2 != 1
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P '[0-9]+\n' -c > TMP2; test TMP2 != 1

	cat 2non_minimal.hwfst | $(top_builddir)/src/hfst-minimize -s -o TMP
	$(top_builddir)/src/hfst-compare 2non_minimal.hwfst TMP 2>1 > /dev/null
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P '[0-9]+\t[0-9]+\t0\t0' -c > TMP2; test TMP2 != 0
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P 'a\ta' -c > TMP2; test TMP2 != 1
	cat TMP | $(top_builddir)/src/hfst-fst2txt | grep -P '[0-9]+\n' -c > TMP2; test TMP2 != 1


hfst-push-weights-functionality-test: 2cat.hfst 2cat_weight_middle.txt 2cat_weight_initial.txt 2cat_weight_final.txt
	$(top_builddir)/src/hfst-push-weights 2cat.hfst -p initial -s | $(top_builddir)/src/hfst-compare -e 2cat.hfst  2>1 > /dev/null
	$(top_builddir)/src/hfst-push-weights 2cat.hfst -p final -s | $(top_builddir)/src/hfst-compare -e 2cat.hfst  2>1 > /dev/null
	$(top_builddir)/src/hfst-txt2fst 2cat_weight_middle.txt -R symbols -w | $(top_builddir)/src/hfst-push-weights -p initial -s | \
	$(top_builddir)/src/hfst-fst2txt -w | diff 2cat_weight_initial.txt -
	$(top_builddir)/src/hfst-txt2fst 2cat_weight_middle.txt -R symbols -w | $(top_builddir)/src/hfst-push-weights -p final -s | \
	$(top_builddir)/src/hfst-fst2txt -w | diff 2cat_weight_final.txt -

hfst-head-functionality-test: cat.hfst dog.hfst catdog.hfst cat.hwfst dog.hwfst catdog.hwfst
	cat cat.hfst dog.hfst > TMP
	cat cat.hfst dog.hfst catdog.hfst | $(top_builddir)/src/hfst-head -s --n-first 2 | \
	$(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null
	cat cat.hfst dog.hfst catdog.hfst > TMP
	cat cat.hfst dog.hfst catdog.hfst | $(top_builddir)/src/hfst-head -s --n-first 5 | \
	$(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null
	cat cat.hwfst dog.hwfst > TMP
	cat cat.hwfst dog.hwfst catdog.hwfst | $(top_builddir)/src/hfst-head -s --n-first 2 | \
	$(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null
	cat cat.hwfst dog.hwfst catdog.hwfst > TMP
	cat cat.hwfst dog.hwfst catdog.hwfst | $(top_builddir)/src/hfst-head -s --n-first 5 | \
	$(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null

hfst-tail-functionality-test: cat.hfst dog.hfst cat.hwfst dog.hwfst
	cat dog.hfst catdog.hfst > TMP
	cat cat.hfst dog.hfst catdog.hfst | $(top_builddir)/src/hfst-tail -s --n-last 2 | \
	$(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null
	cat cat.hfst dog.hfst catdog.hfst > TMP
	cat cat.hfst dog.hfst catdog.hfst | $(top_builddir)/src/hfst-tail -s --n-last 5 | \
	$(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null

	cat dog.hwfst catdog.hwfst > TMP
	cat cat.hwfst dog.hwfst catdog.hwfst | $(top_builddir)/src/hfst-tail -s --n-last 2 | \
	$(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null
	cat cat.hwfst dog.hwfst catdog.hwfst > TMP
	cat cat.hwfst dog.hwfst catdog.hwfst | $(top_builddir)/src/hfst-tail -s --n-last 5 | \
	$(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null

hfst-split-functionality-test:
	cat cat.hfst dog.hfst catdog.hfst > TMP
	cat TMP | $(top_builddir)/src/hfst-split --prefix 'TMP' --extension '.hfst' -s 2>1 > /dev/null
	for i in cat.hfst dog.hfst catdog.hfst; do \
	for j in 1 2 3; do \
	$(top_builddir)/src/hfst-compare -e $$i TMP$$j.hfst  2>1 > /dev/null; \
	rm -f TMP$$j.hfst; \
	done; \
	done

	cat cat.hwfst dog.hwfst catdog.hwfst > TMP
	cat TMP | $(top_builddir)/src/hfst-split --prefix 'TMP' --extension '.hwfst' -s 2>1 > /dev/null
	for i in cat.hwfst dog.hwfst catdog.hwfst; do \
	for j in 1 2 3; do \
	$(top_builddir)/src/hfst-compare -e $$i TMP$$j.hwfst  2>1 > /dev/null; \
	rm -f TMP$$j.hwfst; \
	done; \
	done


hfst-txt2fst-functionality-test: cat.hfst cat.hwfst 4cats.txt
	cat $(srcdir)/4cats.txt | $(top_builddir)/src/hfst-txt2fst -e '<>' | $(top_builddir)/src/hfst-compare cat.hfst -e  2>1 > /dev/null
	cat $(srcdir)/4cats.txt | $(top_builddir)/src/hfst-txt2fst -e '<>' -w | $(top_builddir)/src/hfst-compare cat.hwfst -e  2>1 > /dev/null

hfst-fst2txt-functionality-test:
	cat cat.hfst dog.hfst | $(top_builddir)/src/hfst-fst2txt -w | $(top_builddir)/src/hfst-txt2fst -e '<>' > TMP
	cat cat.hfst dog.hfst | $(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null 
	cat cat.hwfst dog.hwfst | $(top_builddir)/src/hfst-fst2txt -w | $(top_builddir)/src/hfst-txt2fst -w -e '<>' > TMP
	cat cat.hwfst dog.hwfst | $(top_builddir)/src/hfst-compare TMP -e  2>1 > /dev/null 


hfst-symbols-functionality-test: cat.hfst cat.hwfst symbols cat2dog.hfst
	cat cat.hfst | $(top_builddir)/src/hfst-symbols -s | \
	$(top_builddir)/src/hfst-summarize | grep 'has HFST symbol table' | cut -b 33 > TMP; test TMP != 'y'
	cat cat.hfst | $(top_builddir)/src/hfst-symbols -s --do-not-write-symbols --write-symbols-to TMP | \
	$(top_builddir)/src/hfst-summarize | grep 'has HFST symbol table' | cut -b 33 > TMP2; test TMP2 != 'n'
	diff TMP $(srcdir)/symbols
	cat cat.hwfst | $(top_builddir)/src/hfst-symbols -s | \
	$(top_builddir)/src/hfst-summarize | grep 'has HFST symbol table' | cut -b 33 > TMP; test TMP != 'y'
	cat cat.hwfst | $(top_builddir)/src/hfst-symbols -s --do-not-write-symbols --write-symbols-to TMP | \
	$(top_builddir)/src/hfst-summarize | grep 'has HFST symbol table' | cut -b 33 > TMP2; test TMP2 != 'n'
	diff TMP $(srcdir)/symbols
	for i in hfst hwfst; do \
	$(top_builddir)/src/hfst-symbols cat2dog.$$i -s --write-input-symbols-to CAT --write-output-symbols-to DOG > /dev/null; \
	diff CAT $(srcdir)/cat_symbols; \
	diff DOG $(srcdir)/dog_symbols; \
	$(top_builddir)/src/hfst-symbols cat2dog.$$i -s --write-input-symbols-to CAT --write-output-symbols-to DOG -e > /dev/null; \
	diff CAT $(srcdir)/cat_symbols_and_epsilon; \
	diff DOG $(srcdir)/dog_symbols_and_epsilon; \
	\
	$(top_builddir)/src/hfst-symbols cat2dog.$$i -s --write-input-symbols > TMP; \
	$(top_builddir)/src/hfst-symbols TMP -s --write-input-symbols-to CAT > /dev/null; \
	diff CAT $(srcdir)/cat_symbols; \
	$(top_builddir)/src/hfst-symbols cat2dog.$$i -s --write-output-symbols > TMP; \
	$(top_builddir)/src/hfst-symbols TMP -s --write-output-symbols-to DOG > /dev/null; \
	diff DOG $(srcdir)/dog_symbols; \
	\
	$(top_builddir)/src/hfst-symbols cat2dog.$$i -s --write-input-symbols -e > TMP; \
	$(top_builddir)/src/hfst-symbols TMP -s --write-input-symbols-to CAT -e > /dev/null; \
	diff CAT $(srcdir)/cat_symbols_and_epsilon; \
	$(top_builddir)/src/hfst-symbols cat2dog.$$i -s --write-output-symbols -e > TMP; \
	$(top_builddir)/src/hfst-symbols TMP -s --write-output-symbols-to DOG -e > /dev/null; \
	diff DOG $(srcdir)/dog_symbols_and_epsilon; \
	done;


hfst-summarize-functionality-test: summarize_test1.hfst summarize_test2.hfst

#	cat summarize_test1.hfst | $(top_builddir)/src/hfst-summarize -s | diff - summarized1  2>1 > /dev/null
#	cat summarize_test2.hfst | $(top_builddir)/src/hfst-summarize -s | diff - summarized2  2>1 > /dev/null
	cat $(srcdir)/summarized1 $(srcdir)/summarized2 > summarized
	cat summarize_test1.hfst summarize_test2.hfst | $(top_builddir)/src/hfst-summarize -s | diff - summarized  2>1 > /dev/null
	rm summarized


hfst-calculate-functionality-test: symbols calculate_test

	$(top_builddir)/src/hfst-calculate -i $(srcdir)/calculate_test -o /dev/null -s -q -v -C -I  2>1 > /dev/null
	$(top_builddir)/src/hfst-calculate --input $(srcdir)/calculate_test --output /dev/null --silent --quiet --verbose --compatible --invert 2>1 > /dev/null
	cat $(srcdir)/calculate_test | $(top_builddir)/src/hfst-calculate -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hfst-calculate -s $(srcdir)/calculate_test 2>1 > /dev/null # free parameter
	$(top_builddir)/src/hfst-calculate $(srcdir)/calculate_test -R symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/hfst-calculate $(srcdir)/calculate_test --read-symbols $(srcdir)/symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null

hwfst-calculate-functionality-test: symbols calculate_test

	$(top_builddir)/src/hwfst-calculate -i $(srcdir)/calculate_test -o /dev/null -s -q -v -C -I 2>1 > /dev/null
	$(top_builddir)/src/hwfst-calculate --input $(srcdir)/calculate_test --output /dev/null --silent --quiet --verbose --compatible --invert 2>1 > /dev/null
	cat $(srcdir)/calculate_test | $(top_builddir)/src/hfst-calculate -s 2>1 > /dev/null # stdin parameter
	$(top_builddir)/src/hwfst-calculate -s $(srcdir)/calculate_test 2>1 > /dev/null # free parameter
	$(top_builddir)/src/hwfst-calculate $(srcdir)/calculate_test -R $(srcdir)/symbols -D -W TMP 2>1 > /dev/null
	$(top_builddir)/src/hwfst-calculate $(srcdir)/calculate_test --read-symbols $(srcdir)/symbols --do-not-write-symbols --write-symbols-to TMP 2>1 > /dev/null



# Test all short and long commandline-option names.
hfst-pair-test-commandline-test: cat.hfst cat.hfst.names cat_dog_alphabet cat_pairs_spaces
	$(top_builddir)/src/hfst-pair-test -i cat.hfst -o /dev/null -s -v -d -q -n cat.hfst.names -S -p cat_pairs_spaces -R cat_dog_alphabet 2>1 > /dev/null
	$(top_builddir)/src/hfst-pair-test --input cat.hfst --output /dev/null --silent --verbose --debug --quiet --names cat.hfst.names --spaces --pairs cat_pairs_spaces --read-symbols cat_dog_alphabet 2>1 > /dev/null
	cat cat.hfst | $(top_builddir)/src/hfst-pair-test -s -S -p cat_pairs_spaces
	$(top_builddir)/src/hfst-pair-test -s -S -p cat_pairs_spaces cat.hfst
	$(top_builddir)/src/hfst-pair-test -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-pair-test --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-pair-test -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-pair-test --version 2>1 > /dev/null

hfst-diff-test-commandline-test: cat.hfst cat_alphabet cat_sets_spaces
	$(top_builddir)/src/hfst-diff-test -i cat.hfst -o /dev/null -s -v -d -q -S -r $(srcdir)/cat_sets_spaces -R cat_alphabet 2>1 > /dev/null
	$(top_builddir)/src/hfst-diff-test --input cat.hfst --output /dev/null --silent --verbose --debug --quiet --spaces --relation $(srcdir)/cat_sets_spaces --read-symbols cat_alphabet 2>1 > /dev/null
	cat cat.hfst | $(top_builddir)/src/hfst-diff-test -s -S -r $(srcdir)/cat_sets_spaces
	$(top_builddir)/src/hfst-diff-test -s -S -r $(srcdir)/cat_sets_spaces cat.hfst
	$(top_builddir)/src/hfst-diff-test -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-diff-test --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-diff-test -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-diff-test --version 2>1 > /dev/null


hfst-compose-intersect-commandline-test: cat.hfst cat_alphabet aA.hfst tT.hfst
	$(top_builddir)/src/hfst-compose-intersect -q -s -v -l cat.hfst -o /dev/null -R cat_alphabet -W /dev/null aA.hfst tT.hfst
	$(top_builddir)/src/hfst-compose-intersect --quiet --silent --verbose --lexicon cat.hfst --output /dev/null --read-symbols cat_alphabet --write-symbols /dev/null aA.hfst tT.hfst
	cat cat.hfst aA.hfst tT.hfst | $(top_builddir)/src/hfst-compose-intersect -s 2>1 > /dev/null
	$(top_builddir)/src/hfst-compose-intersect -h 2>1 > /dev/null
	$(top_builddir)/src/hfst-compose-intersect --help 2>1 > /dev/null
	$(top_builddir)/src/hfst-compose-intersect -V 2>1 > /dev/null
	$(top_builddir)/src/hfst-compose-intersect --version 2>1 > /dev/null


hfst-fst2strings-functionality-test: ccCaAtT.hfst dog.hfst cat_dog_strings cat_dog_pair_strings cat_dog_strings_spaces cat_dog_pair_strings_spaces 
	cat ccCaAtT.hfst dog.hfst | $(top_builddir)/src/hfst-fst2strings | \
	sort > cat_dog_strings.1
	cat $(srcdir)/cat_dog_strings | sort | diff - cat_dog_strings.1
	rm -f cat_dog_strings.1
	cat ccCaAtT.hfst dog.hfst | $(top_builddir)/src/hfst-fst2strings -p | \
	sort > cat_dog_pair_strings.1
	cat $(srcdir)/cat_dog_pair_strings | sort | diff - cat_dog_pair_strings.1
	rm -f cat_dog_pair_strings.1
	cat ccCaAtT.hfst dog.hfst | $(top_builddir)/src/hfst-fst2strings -S | \
	sort > cat_dog_strings_spaces.1
	cat $(srcdir)/cat_dog_strings_spaces | sort | diff - cat_dog_strings_spaces.1
	rm -f cat_dog_strings_spaces.1
	cat ccCaAtT.hfst dog.hfst | $(top_builddir)/src/hfst-fst2strings -p -S | \
	sort > cat_dog_pair_strings_spaces.1
	cat $(srcdir)/cat_dog_pair_strings_spaces | sort | diff - cat_dog_pair_strings_spaces.1
	rm -f cat_dog_pair_strings_spaces.1


hfst-strings2fst-functionality-test: ccCaAtT.hfst dog.hfst cat_dog_strings cat_dog_pair_strings cat_dog_strings_spaces cat_dog_pair_strings_spaces cat_or_dog.hfst cat_or_dog.hwfst cat+dog.hfst cat+dog.hwfst
	cat $(srcdir)/cat_dog | $(top_builddir)/src/hfst-strings2fst -j -R $(srcdir)/cat_dog_alphabet -o cat_or_dog.hfst1
	cat cat_dog | $(top_builddir)/src/hfst-strings2fst -w -j -R $(srcdir)/cat_dog_alphabet -o cat_or_dog.hwfst1
	cat $(srcdir)/cat_dog | $(top_builddir)/src/hfst-strings2fst -R $(srcdir)/cat_dog_alphabet -o cat+dog.hfst1
	cat $(srcdir)/cat_dog | $(top_builddir)/src/hfst-strings2fst -w -R $(srcdir)/cat_dog_alphabet -o cat+dog.hwfst1 
	$(top_builddir)/src/hfst-compare cat_or_dog.hfst cat_or_dog.hfst1
	$(top_builddir)/src/hfst-compare cat_or_dog.hwfst cat_or_dog.hwfst1
	$(top_builddir)/src/hfst-compare cat+dog.hfst cat+dog.hfst1
	$(top_builddir)/src/hfst-compare cat+dog.hwfst cat+dog.hwfst1
	rm -f cat_or_dog.hfst1 cat_or_dog.hwfst1 cat+dog.hfst1 cat+dog.hwfst1


hfst-strings-functionality-test:
	$(top_builddir)/src/hfst-strings2fst $(srcdir)/animals_and_frequencies -R $(srcdir)/symbols -w -j --norm --log -o animals.hwfst
	$(top_builddir)/src/hfst-fst2strings animals.hwfst --nbest 1 -o TMP
	head -1 $(srcdir)/animals | diff TMP - 
	$(top_builddir)/src/hfst-fst2strings animals.hwfst --nbest 2 | sort > TMP
	head -2 $(srcdir)/animals | sort | diff TMP - 
	$(top_builddir)/src/hfst-fst2strings animals.hwfst --nbest 3 | sort > TMP
	head -3 $(srcdir)/animals | sort | diff TMP -  
	$(top_builddir)/src/hfst-fst2strings animals.hwfst --nbest 4 | sort > TMP
	head -4 $(srcdir)/animals | sort | diff TMP - 
	$(top_builddir)/src/hfst-fst2strings animals.hwfst | sort > TMP 
	sort $(srcdir)/animals | diff TMP - 
	rm -f TMP

#hfst-lookup-crash-test: CcAaTt_.hfst cat_sets
#	$(CRASH_TST) $(top_builddir)/src/hfst-lookup -q -i CcAaTt_.hfst -I cat_sets > /dev/null

%.hfst: %.txt symbols
	cat $< | $(top_builddir)/src/hfst-txt2fst -R $(srcdir)/symbols > $@

%.hwfst: %.txt symbols
	cat $< | $(top_builddir)/src/hfst-txt2fst -w -R $(srcdir)/symbols > $@


2%.txt: %.txt
	cat $^ $(srcdir)/separator $^ > $@
