#!/bin/sh
for i in "" .sfst .ofst .foma; do 
    if test -f cat.hfst$i -a -f cat2dog.hfst$i ; then
        if ! ../../tools/src/hfst-compose cat.hfst$i cat2dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat2dog.hfst$i > /dev/null 2>&1 ; then
            exit 1
        fi
        rm test.hfst;
    fi
done
