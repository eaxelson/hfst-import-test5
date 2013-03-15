#!/bin/sh

XFST_TOOL="../hfst-xfst2fst -s"
STRINGS2FST="../../hfst-strings2fst -S"
TXT2FST="../../hfst-txt2fst"
COMPARE="../../hfst-compare --quiet"
LIST_FORMATS="../../hfst-format --list-formats"
EXTRA_FILES="tmp startup"
REMOVE="rm -f"

for format in sfst openfst-tropical foma;
do
    # Test if implementation type is available.
    if ! (${LIST_FORMATS} | grep $format > /dev/null); then
	continue;
    fi

    ## Create a transducer [Foo Bar Baz] where Foo is [foo], Bar [bar] and Baz [Baz].
    ## Definition of Foo is given in startup file, and definitions of Bar and Baz
    ## on command line. Baz is later undefined in input.
    echo "define Foo" > startup # continue regex on
    echo "foo;" >> startup      # another line
    if ! ((echo "undefine Baz" && echo "regex Foo Bar Baz;" && echo "save stack tmp") | \
	${XFST_TOOL} -f $format -l startup \
	-e "define Bar bar;" -e "define Baz baz;" > /dev/null 2> /dev/null);
    then
	${REMOVE} ${EXTRA_FILES}
	exit 1
    fi
    # Test that the result is as intended.
    if ! (echo "foo bar Baz" | ${STRINGS2FST} -f $format | ${COMPARE} tmp);
    then
	${REMOVE} ${EXTRA_FILES}
	exit 1
    fi

    # Create a transducer with literal words "define" and "regex".
    for word in define regex
    do
	if ! ((echo "regex "$word";" && echo "save stack tmp") | ${XFST_TOOL} -f $format > /dev/null 2> /dev/null;);
	then
	    ${REMOVE} ${EXTRA_FILES}
	    exit 1
	fi
        # Test that the result is as intended.
	if ! (echo $word | ${STRINGS2FST} -f $format | ${COMPARE} tmp);
	then
	    ${REMOVE} ${EXTRA_FILES}
	    exit 1
	fi
    done

    ## Test that using special symbols in replace rules yields an error message
    if ! (echo 'regex a -> "@_foo_@";' | ../hfst-xfst2fst -f $format > /dev/null 2> tmp && grep "warning:" tmp > /dev/null); then
	exit 1;
    fi
    # silent mode
    if (echo 'regex a -> "@_foo_@";' | ../hfst-xfst2fst -s -f $format > /dev/null 2> tmp && grep "warning:" tmp > /dev/null); then
	exit 1;
    fi

    ## Test that the result of testfile.xfst (written in att format in file 'result')
    ## is the same as testfile.att.
    for testfile in compose_net concatenate_net union_net ignore_net invert_net minus_net intersect_net \
	determinize_net epsilon_remove_net invert_net minimize_net negate_net \
	one_plus_net prune_net reverse_net sort_net upper_side_net zero_plus_net lower_side_net
    do
	if ! (ls $testfile.xfst 2> /dev/null); then
	    echo "skipping missing test for "$testfile"..."
	    continue
	fi
	if ! (cat $testfile.xfst | ../hfst-xfst2fst -q -f $format > /dev/null); then
	    echo "ERROR: in compiling "$testfile".xfst"
	    exit 1;
	fi
	if ! (cat result | ${TXT2FST} > tmp1; cat $testfile.att | ${TXT2FST} > tmp2; ); then
	    echo "ERROR: in compiling "$testfile".att"
	    exit 1;
	fi
	if ! (${COMPARE} tmp1 tmp2); then
	    echo "ERROR: "$testfile" test failed"
	    exit 1;
	fi
    done

    ## Test that the result of testfile.xfst (written to standard output)
    ## contains the lines listed in testfile.grep.
    for testfile in apply_up apply_down print_stack
    do
	if ! (ls $testfile.xfst 2> /dev/null); then
	    echo "skipping missing test for "$testfile"..."
	    continue
	fi
	if ! (cat $testfile.xfst | ../hfst-xfst2fst -f $format > tmp); then
	    echo "ERROR: in compiling "$testfile.xfst
	    exit 1;
	fi
	for expression in `cat $testfile.grep`
	do
	    if ! (grep $expression tmp > /dev/null); then
		echo "ERROR: "$testfile" test failed: cannot find '"$expression"' in output"
		exit 1;
	    fi
	done
    done

## add properties
# alias
# apropos
# cleanup net
# clear stack
# collect epsilon-loops
# compact sigma
# compile-replace lower
# compile-replace upper
# complete net
# crossproduct net
# echo
# edit properties
# eliminate flag
# help
# ignore net
# inspect net
# label net
# list
# load defined
# load stack
# name net
# pop stack
# print aliases
# print arc-tally
# print defined
# print directory
# print file-info
# print flags
# print label-maps
# print labels
# print label-tally
# print list
# print lists
# print longest-string
# print longest-string-size
# print lower-words
# print name
# print net
# print random-lower
# print random-upper
# print random-words
# print shortest-string
# print shortest-string-size
# print sigma
# print sigma-tally
# print sigma-word-tally
# print size
# print upper-words
# print words
# push defined
# quit
# read att
# read lexc
# read prolog
# read properties
# read regex
# read spaced-text
# read text
# rotate stack
# save defined
# save stack
# set
# show
# shuffle net
# sigma net
# source
# substitute defined
# substitute label
# substitute symbol
# substring net
# system
# test equivalent
# test lower-bounded
# test lower-universal
# test non-null
# test null
# test overlap
# test sublanguage
# test upper-bounded
# test upper-universal
# turn stack
# twosided flag-diacritics
# undefine
# unlist
# write att
# write definition
# write definitions
# write dot
# write prolog
# write properties
# write spaced-text
# write text



done

${REMOVE} ${EXTRA_FILES}
exit 0;

