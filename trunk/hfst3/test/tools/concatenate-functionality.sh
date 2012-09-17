#!/bin/sh
TOOLDIR=../../tools/src

for i in "" .sfst .ofst .foma; do
    if test -f cat$i -a -f dog$i ; then
        if ! $TOOLDIR/hfst-concatenate cat$i dog$i > test ; then
            exit 1
        fi
        if ! $TOOLDIR/hfst-compare -s test catdog$i  ; then
            exit 1
        fi
        rm test;
    fi
done
