#!/bin/sh
for i in "" .sfst .ofst .foma; do
if ! ../../tools/src/hfst-summarize cat.hfst$i > test.txt ; then
    exit 1
fi
rm test.txt;
done
