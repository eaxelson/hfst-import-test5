#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f non_minimal.hfst$i ; then
        if ! ../../tools/src/hfst-determinize non_minimal.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test.hfst non_minimal.hfst$i  ; then
            exit 1
        fi
        rm test.hfst;
    fi
done
