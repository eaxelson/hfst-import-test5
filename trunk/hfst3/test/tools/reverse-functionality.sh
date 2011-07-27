#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a -f dog2cat.hfst$i ; then
        if ! ../../tools/src/hfst-invert cat2dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst dog2cat.hfst$i  ; then
            exit 1
        fi
        rm test.hfst;
    fi
done
