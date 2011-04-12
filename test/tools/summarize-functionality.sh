#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i ; then
        if ! ../../tools/src/hfst-summarize cat.hfst$i > test.txt ; then
            exit 1
        fi
        rm test.txt;
    fi
done
