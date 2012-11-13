#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat_or_dog$i -a -f cat$i ; then
        if ! ../../tools/src/hfst-conjunct cat_or_dog$i cat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat$i  ; then
            exit 1
        fi
        rm test;
    fi
done
