#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f non_minimal$i ; then
        if ! ../../tools/src/hfst-determinize non_minimal$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test non_minimal$i  ; then
            exit 1
        fi
        rm test;
    fi
done
