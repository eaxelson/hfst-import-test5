#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat$i -a -f 2to4cats$i -a -f 0to3cats$i \
        -a -f 4cats$i -a -f 4toINFcats$i ; then
        if ! ../../tools/src/hfst-repeat -f 2 -t 4 cat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test 2to4cats$i  ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-repeat -t 3 cat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test 0to3cats$i  ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-repeat -f 4 -t 4 cat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test 4cats$i  ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-repeat -f 4 cat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test 4toINFcats$i  ; then
            exit 1
        fi
        rm test;
    fi
done
