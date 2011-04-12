#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i ; then
        if ! ../../tools/src/hfst-fst2txt -D < cat.hfst$i > test.txt ; then
            exit 1
        fi
        if ! diff test.txt cat.txt > /dev/null 2>&1 ; then
            exit 1
        fi
        rm test.txt;
    fi
done
