#!/bin/sh

XFST_TOOL="../hfst-xfst2fst -s"
STRINGS2FST="../../hfst-strings2fst -S"
COMPARE="../../hfst-compare --quiet"
FORMATS="../../hfst-format --list-formats"
EXTRA_FILES="tmp startup"
REMOVE="rm -f"

for format in sfst openfst-tropical foma;
do
    if ! (${FORMATS} --list-formats | grep $format > /dev/null); then
	continue;
    fi
    echo "define Foo foo;" > startup
    if ! (echo "regex Foo Bar; save stack tmp;" | ${XFST_TOOL} -f $format -l startup -e "define Bar bar;"); then
	${REMOVE} ${EXTRA_FILES}
	exit 1
    fi
    if ! (echo "foo bar" | ${STRINGS2FST} -f $format -S | ${COMPARE} tmp); then
	${REMOVE} ${EXTRA_FILES}
	exit 1
    fi
done

${REMOVE} ${EXTRA_FILES}
exit 0;
