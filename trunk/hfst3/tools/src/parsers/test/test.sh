#!/bin/sh

#exit 77;

XFST_TOOL="../hfst-xfst2fst -s --pipe-mode"
STRINGS2FST="../../hfst-strings2fst -S"
TXT2FST="../../hfst-txt2fst"
COMPARE="../../hfst-compare --quiet"
DIFF="diff --ignore-blank-lines"
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
	-e "define Bar bar;" -e "define Baz baz;" > /dev/null 2> /dev/null)
    then
	${REMOVE} ${EXTRA_FILES}
        echo "fail #1";
	exit 1
    fi

    # Test that the result is as intended.
    if ! (echo "foo bar Baz" | ${STRINGS2FST} -f $format | ${COMPARE} tmp);
    then
	${REMOVE} ${EXTRA_FILES}
        echo "fail #2";
	exit 1
    fi

    # Create a transducer with literal words "define" and "regex".
    for word in define regex
    do
	if ! ((echo "regex "$word";" && echo "save stack tmp") | ${XFST_TOOL} -f $format > /dev/null 2> /dev/null)
	then
	    ${REMOVE} ${EXTRA_FILES}
            echo "fail #3";
	    exit 1
	fi
        # Test that the result is as intended.
	if ! (echo $word | ${STRINGS2FST} -f $format | ${COMPARE} tmp);
	then
	    ${REMOVE} ${EXTRA_FILES}
            echo "fail #4";
	    exit 1
	fi
    done

    ## Test that using special symbols in replace rules yields an error message
    if ! (echo 'regex a -> "@_foo_@";' | ../hfst-xfst2fst --pipe-mode -f $format > /dev/null 2> tmp && grep "warning:" tmp > /dev/null); then
        echo "fail #5";
	exit 1;
    fi
    # silent mode
    if (echo 'regex a -> "@_foo_@";' | ../hfst-xfst2fst --pipe-mode -s -f $format > /dev/null 2> tmp && grep "warning:" tmp > /dev/null); then
        echo "fail #6";
	exit 1;
    fi

    ## Test that the transducer info is correct
    if ! (echo 'regex [a|b|c|d|e] ([d|e|f|g]);' | ../hfst-xfst2fst --pipe-mode -f $format > tmp 2> /dev/null); then
        echo "fail #7";
        exit 1;
    fi
    if (! grep "3 states" tmp > /dev/null); then
        echo "here 1"
        exit 1;
    fi
    if (! grep "9 arcs" tmp > /dev/null); then
        echo "here 2"
        exit 1;
    fi

    ## Test that the result of testfile.xfst (written in att format to standard output)
    ## is the same as testfile.att using att-to-fst conversion.
    for testfile in compose_net concatenate_net union_net ignore_net invert_net minus_net intersect_net \
	determinize_net epsilon_remove_net invert_net minimize_net negate_net \
	one_plus_net prune_net reverse_net sort_net upper_side_net zero_plus_net lower_side_net \
	define define_function prolog # substitute_symbol substitute_label substitute_defined
    do
	rm -f result result1 result2
	if ! (ls $testfile.xfst 2> /dev/null); then
	    echo "skipping missing test for "$testfile"..."
	    continue
	fi
	if ! (cat $testfile.xfst | ../hfst-xfst2fst --pipe-mode -q -f $format > result 2> /dev/null); then
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

    ## Test that testfile_fail fails.
    #for testfile in define_fail
    #do
#	if ! (ls $testfile.xfst 2> /dev/null); then
#	    echo "skipping missing test for "$testfile"..."
#	    continue
#	fi
#	if ! (cat $testfile.xfst | ../hfst-xfst2fst -s -f $format 2> tmp > /dev/null); then
#	    echo "ERROR: in compiling "$testfile".xfst"
#	    exit 1;
#	fi
#	if ! (grep "xre parsing failed" tmp > /dev/null); then
#	    echo "ERROR: in "$testfile".xfst"
#	    exit 1;
#	fi
#    done

    ## Test that the result of testfile.xfst (written to standard output)
    ## is the same as testfile.output
    for testfile in print_stack print_labels print_label_tally \
	shortest_string set_variable eliminate_flag info print_net
    do
	if ! (ls $testfile.xfst 2> /dev/null); then
	    echo "skipping missing test for "$testfile"..."
	    continue
	fi
        # apply up/down leak to stdout with readline..
	if ! (cat $testfile.xfst | ../hfst-xfst2fst --pipe-mode -f $format -s > tmp); then
	    echo "ERROR: in compiling "$testfile.xfst
	    exit 1;
	fi
	if ! ($DIFF tmp $testfile.output > tmpdiff); then
            if (ls $testfile.alternative_output > /dev/null 2> /dev/null); then
                if ! ($DIFF tmp $testfile.alternative_output); then
                    rm -f tmpdiff
                    echo "ERROR: in result from "$testfile.xfst
                    exit 1;
                fi
            else
                cat tmpdiff
                rm -f tmpdiff
	        echo "ERROR: in result from "$testfile.xfst
	        exit 1;
            fi
            rm -f tmpdiff
	fi
    done

    ## Interactive commands
    for testfile in apply_up apply_down inspect_net
    do
	if ! (ls $testfile.xfst 2> /dev/null); then
	    echo "skipping missing test for "$testfile"..."
	    continue
	fi
        # apply up/down leak to stdout with readline..
        for param in --pipe-mode --no-readline
        do
            # 'inspect net' requires input from stdin
            if (test "$param" = "--pipe-mode" -a "$testfile" = "inspect_net"); then
                continue
            fi
	    if ! (cat $testfile.xfst | ../hfst-xfst2fst $param -f $format -s > tmp); then
	    echo "ERROR: in compiling "$testfile.xfst" with parameters "$param
	    exit 1;
	    fi
	    if ! ($DIFF tmp $testfile.output > tmpdiff); then
                if (ls $testfile.alternative_output > /dev/null 2> /dev/null); then
                    if ! ($DIFF tmp $testfile.alternative_output); then
                        rm -f tmpdiff
                        echo "ERROR: in result from "$testfile.xfst
                        exit 1;
                    fi
                else
                    cat tmpdiff
                    rm -f tmpdiff
	            echo "ERROR: in result from "$testfile.xfst
	            exit 1;
                fi
                rm -f tmpdiff
	    fi
        done
    done


    ## Test that the results of testfile_true.xfst and testfile_false.xfst (written to file tmp)
    ## contain the lines listed in files test_true.input and test_false.output, respectively.
    for testcase in _true _false # whether we test the positive or negative case
    do
	for testfile in test_overlap test_sublanguage # the function to be tested
	do
	    if ! (ls $testfile$testcase.xfst 2> /dev/null); then
		echo "skipping missing test for "$testfile$testcase"..."
		continue
	    fi
	    if ! (cat $testfile$testcase.xfst | ../hfst-xfst2fst --pipe-mode -s -f $format > tmp); then
		echo "ERROR: in compiling "$testfile$testcase.xfst
		exit 1;
	    fi
	    if ! ($DIFF tmp "test"$testcase.output); then
		echo "ERROR: in testing "$testfile$testcase.xfst
		exit 1;
	    fi
	done
    done

    for file in quit-on-fail.xfst assert.xfst
    do
        if (cat $file | ../hfst-xfst2fst -s -f $format > tmp 2> /dev/null); then
            echo "ERROR: in compiling "$file
            exit 1;
        fi
        if (grep '^fail' tmp > /dev/null); then
            echo "ERROR: in testing "$file
            exit 1;
        fi
        if ! (grep '^pass' tmp > /dev/null); then
            echo "ERROR: in testing "$file
            exit 1;
        fi
    done

    rm -f result tmp1 tmp2 foo

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

