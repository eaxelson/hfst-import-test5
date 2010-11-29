#!/bin/sh
for i in "" .sfst .ofst .foma; do
if ! cat cat2dog.hfst$i dog2cat.hfst$i | ../../tools/src/hfst-tail -n 1 > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst dog2cat.hfst$i > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst;
done
