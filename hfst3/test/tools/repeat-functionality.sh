#!/bin/sh
for i in "" .sfst .ofst .foma; do
if ! ../../tools/src/hfst-repeat -f 2 -t 4 cat.hfst$i > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst 2to4cats.hfst$i > /dev/null 2>&1 ; then
    exit 1
fi
if ! ../../tools/src/hfst-repeat -t 3 cat.hfst$i > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst 0to3cats.hfst$i > /dev/null 2>&1 ; then
    exit 1
fi
if ! ../../tools/src/hfst-repeat -f 4 -t 4 cat.hfst$i > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst 4cats.hfst$i > /dev/null 2>&1 ; then
    exit 1
fi
if ! ../../tools/src/hfst-repeat -f 4 cat.hfst$i > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst 4toINFcats.hfst$i > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst;
done
