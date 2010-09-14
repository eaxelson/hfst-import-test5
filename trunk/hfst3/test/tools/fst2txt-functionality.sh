#!/bin/sh
if ! ../../tools/src/hfst-fst2txt -D < cat.hfst > test.txt ; then
    exit 1
fi
if ! diff test.txt cat.txt > /dev/null 2>&1 ; then
    exit 1
fi
rm test.txt
