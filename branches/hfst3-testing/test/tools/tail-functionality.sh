#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog$i -a -dog2cat$i ; then
        if ! cat cat2dog$i dog2cat$i | ../../tools/src/hfst-tail -n 1 > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test dog2cat$i  ; then
            exit 1
        fi
        rm test;
        if ! cat cat2dog$i dog2cat$i | ../../tools/src/hfst-tail -n +2 > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test dog2cat$i  ; then
            exit 1
        fi
        rm test;
    fi
done
