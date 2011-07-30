#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a -f dog2cat.hfst$i ; then
        if ! cat cat2dog.hfst$i dog2cat.hfst$i | ../../tools/src/hfst-head -n 1 > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat2dog.hfst$i  ; then
            exit 1
        fi
        rm test.hfst;
    fi
done
