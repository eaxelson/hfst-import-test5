#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i -a -f 2to4cats.hfst$i -a -f 0to3cats.hfst$i \
        -a -f 4cats.hfst$i -a -f 4toINFcats.hfst$i ; then
        if ! ../../tools/src/hfst-repeat -f 2 -t 4 cat.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test.hfst 2to4cats.hfst$i  ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-repeat -t 3 cat.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test.hfst 0to3cats.hfst$i  ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-repeat -f 4 -t 4 cat.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test.hfst 4cats.hfst$i  ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-repeat -f 4 cat.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test.hfst 4toINFcats.hfst$i  ; then
            exit 1
        fi
        rm test.hfst;
    fi
done
