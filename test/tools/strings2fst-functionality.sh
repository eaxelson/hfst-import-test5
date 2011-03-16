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
    if test -f cat.hfst$i ; then
        if ! ../../tools/src/hfst-strings2fst $FFLAG < cat.strings > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat.hfst$i > /dev/null 2>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-strings2fst $FFLAG -S < c_a_t.strings > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat.hfst$i > /dev/null 2>&1 ; then
            exit 1
        fi
        rm test.hfst
    fi
    if test -f cat2dog.hfst$i ; then
        if ! ../../tools/src/hfst-strings2fst $FFLAG < cat2dog.strings > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat2dog.hfst$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-strings2fst $FFLAG -S < cat2dog.spaces > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat2dog.hfst$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-strings2fst $FFLAG -p -S < cat2dog.pairs > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat2dog.hfst$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-strings2fst $FFLAG -p < cat2dog.pairstring > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat2dog.hfst$i > /dev/null 1>&1 ; then
            exit 1
        fi
    fi
done

