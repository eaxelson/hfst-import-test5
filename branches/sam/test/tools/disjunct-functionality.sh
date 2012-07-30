#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat$i -a -f dog$i ; then
        if ! ../../tools/src/hfst-disjunct cat$i dog$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat_or_dog$i  ; then
            exit 1
        fi
        rm test;
    fi
done