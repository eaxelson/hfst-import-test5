#!/bin/sh
for i in "" .sfst .ofst .foma; do
if ! ../../tools/src/hfst-substitute cat.hfst$i -F cat2dog.substitute > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst dog.hfst$i > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst;
done
