#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat_or_dog.hfst$i -a -f dog.hfst$i -a cat.hfst$i ; then
        if ! ../../tools/src/hfst-subtract cat_or_dog.hfst$i dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat.hfst$i > /dev/null 2>&1 ; then
            exit 1
        fi
        rm test.hfst;
    fi
done
