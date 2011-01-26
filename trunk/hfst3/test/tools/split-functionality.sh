#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a -f dog2cat.hfst$i ; then
        if ! cat cat2dog.hfst$i dog2cat.hfst$i | ../../tools/src/hfst-split ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare 1.hfst cat2dog.hfst$i > /dev/null 2>&1 ; then
            rm 1.hfst 2.hfst
            exit 1
        fi
        if ! ../../tools/src/hfst-compare 2.hfst dog2cat.hfst$i > /dev/null 2>&1 ; then
            rm 1.hfst 2.hfst
            exit 1
        fi
        rm 1.hfst 2.hfst;
    fi
done
