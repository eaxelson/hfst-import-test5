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

    ## Create a transducer [Foo Bar Baz] where Foo is [foo], Bar [bar] and Baz [baz].
    ## Definition of Foo is given in startup file, and definitions of Bar and Baz
    ## on command line.
    echo "define Foo" > startup # continue regex on
    echo "foo;" >> startup      # another line
    if ! ((echo "regex Foo Bar Baz;" && echo "save stack tmp") | \
	${XFST_TOOL} -f $format -l startup \
	-e "define Bar bar;" -e "define Baz baz;");
    then
	${REMOVE} ${EXTRA_FILES}
	exit 1
    fi
    # Test that the result is as intended.
    if ! (echo "foo bar baz" | ${STRINGS2FST} -f $format | ${COMPARE} tmp);
    then
	${REMOVE} ${EXTRA_FILES}
	exit 1
    fi

    # Create a transducer with literal words "define".
    if ! ((echo "regex define;" && echo "save stack tmp") | ${XFST_TOOL} -f $format;);
    then
	${REMOVE} ${EXTRA_FILES}
	exit 1
    fi
    # Test that the result is as intended.
    if ! (echo "define" | ${STRINGS2FST} -f $format | ${COMPARE} tmp);
    then
	${REMOVE} ${EXTRA_FILES}
	exit 1
    fi

    ## Test that using special symbols in replace rules yields an error message
    if ! (echo 'regex a -> "@_foo_@";' | ../hfst-xfst2fst -f $format > /dev/null 2> tmp && grep "warning:" tmp); then
	exit 1;
    fi
    # silent mode
    if (echo 'regex a -> "@_foo_@";' | ../hfst-xfst2fst -s -f $format > /dev/null 2> tmp && grep "warning:" tmp); then
	exit 1;
    fi

    for testfile in compose_net concatenate_net union_net ignore_net invert_net minus_net intersect_net;
    do
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

    # for testfile in test_equivalence

## add properties
# alias
# apply down
# apply up
# apropos
# cleanup net
# clear stack
# collect epsilon-loops
# compact sigma
# compile-replace lower
# compile-replace upper
# complete net
# compose net


# concatenate net
# crossproduct net
# define
# determinize net
# echo
# edit properties
# eliminate flag
# epsilon-remove net
# help
# ignore net
# inspect net
# intersect net
# invert net
# label net
# list
# load defined
# load stack
# lower-side net
# minimize net
# minus net
# name net
# negate net
# one-plus net
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
# print stack
# print upper-words
# print words
# prune net
# push defined
# quit
# read att
# read lexc
# read prolog
# read properties
# read regex
# read spaced-text
# read text
# reverse net
# rotate stack
# save defined
# save stack
# set
# show
# shuffle net
# sigma net
# sort net
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
# union net
# unlist
# upper-side net
# write att
# write definition
# write definitions
# write dot
# write prolog
# write properties
# write spaced-text
# write text
# zero-plus net


done

${REMOVE} ${EXTRA_FILES}
exit 0;

