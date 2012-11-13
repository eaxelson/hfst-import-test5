#!/bin/sh
for i in "" .sfst .ofst .foma; do
    FFLAG=
    case $i in
        .sfst)
            FFLAG="-f sfst";;
        .ofst)
            FFLAG="-f openfst-tropical";;
        .foma)
            FFLAG="-f foma";;
        *)
            FFLAG=;;
    esac
    if test -f cat$i ; then
        if ! ../../tools/src/hfst-txt2fst $FFLAG $srcdir/cat.txt > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat$i  ; then
            exit 1
        fi
        rm test
    fi
done
