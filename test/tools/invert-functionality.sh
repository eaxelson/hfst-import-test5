#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog$i ; then
        if ! ../../tools/src/hfst-invert cat2dog$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test dog2cat$i  ; then
            exit 1
        fi
        rm test;
    fi
done
