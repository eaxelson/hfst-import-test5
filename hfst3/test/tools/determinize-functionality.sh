#!/bin/sh
for i in "" .sfst .ofst .foma; do
if ! ../../tools/src/hfst-determinize non_minimal.hfst$i > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst non_minimal.hfst$i > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst;
done
