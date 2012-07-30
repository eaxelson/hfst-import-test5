#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i ; then
        if ! ../src/hfst-invert cat2dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst dog2cat.hfst$i  ; then
            exit 1
        fi
        rm test.hfst;
    fi
done
