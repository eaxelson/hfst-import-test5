#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a -dog2cat.hfst$i ; then
        if ! cat cat2dog.hfst$i dog2cat.hfst$i | ../../tools/src/hfst-tail -n 1 > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test.hfst dog2cat.hfst$i  ; then
            exit 1
        fi
        rm test.hfst;
    fi
done