#!/bin/sh
#
# Test that the installed version of hfst-xfst works.
#

if ! [ -x $1/hfst-xfst ]; then
    echo "warning: hfst-xfst not found, assumed switched off and skipping it"
    exit 0
fi

rm -f a2b.script
echo "regex a:b;" > a2b.script
echo "save stack a2b.hfst" >> a2b.script 

for format in sfst openfst-tropical foma;
do
    # Check that the tool is installed right..
    if ! ($1/hfst-xfst -f a2b.script -F $format 2>1 > /dev/null); then
	exit 1
    fi
    # Check that the tool produces right result..
    if ! (echo "a:b" | $1/hfst-strings2fst -f $format | $1/hfst-compare -s a2b.hfst); then
	exit 1
    fi
    rm -f a2b.hfst
done

rm -f a2b.script
