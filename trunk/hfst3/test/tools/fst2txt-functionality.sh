#!/bin/sh

if [ "$srcdir" = "" ]; then
    srcdir="./";
fi

for i in "" .sfst .ofst .foma; do
    if test -f cat$i ; then
        if ! ../../tools/src/hfst-fst2txt -D < cat$i > test.txt ; then
            exit 1
        fi
        if ! diff test.txt $srcdir/cat.txt > /dev/null 2>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-fst2txt -f dot < cat$i > test.txt ; then
            exit 1
        fi
        if which dot > /dev/null 2>&1 ; then
            if ! dot test.txt > /dev/null 2>&1 ; then
                exit 1
            fi
        fi
        if ! ../../tools/src/hfst-fst2txt -f pckimmo < cat$i > test.txt ; then
            exit 1
        fi
        rm test.txt
    fi
done
