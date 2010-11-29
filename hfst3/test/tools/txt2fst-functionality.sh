#!/bin/sh
if ! ../../tools/src/hfst-txt2fst -f sfst < cat.txt > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst.sfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst

if ! ../../tools/src/hfst-txt2fst -f openfst-tropical < cat.txt > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst.ofst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst

if ! ../../tools/src/hfst-txt2fst -f foma < cat.txt > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst.foma > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
