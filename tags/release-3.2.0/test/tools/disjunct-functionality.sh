#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i -a -f dog.hfst$i ; then
        if ! ../../tools/src/hfst-disjunct cat.hfst$i dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat_or_dog.hfst$i  ; then
            exit 1
        fi
        rm test.hfst;
    fi
done