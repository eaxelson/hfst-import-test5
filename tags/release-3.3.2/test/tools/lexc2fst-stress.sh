#!/bin/sh
LEXCTESTS="stress.random-lexicons-100.lexc"
for i in "" .sfst .ofst .foma ; do
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
    for f in $LEXCTESTS ; do
        if ! ../../tools/src/hfst-lexc2fst $FFLAG $srcdir/$f > test.hfst ; then
            exit 1
        fi
        rm test.hfst
    done
done
