#!/bin/sh

XFST_TOOL="../hfst-xfst2fst -s"
STRINGS2FST="../../hfst-strings2fst -S"
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

    # Create a transducer [Foo Bar Baz] where Foo is [foo], Bar [bar] and Baz [baz].
    # Definition of Foo is given in startup file, and definitions of Bar and Baz
    # on command line.
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
    
done

${REMOVE} ${EXTRA_FILES}
exit 0;
