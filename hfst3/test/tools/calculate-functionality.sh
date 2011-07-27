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
    if test -f 4toINFcats.hfst$i ; then
        if ! echo "catcatcat(cat)+" | ../../tools/src/hfst-calculate $FFLAG > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare 4toINFcats.hfst$i test.hfst  ; then
            exit 1
        fi
        rm test.hfst
    fi
    if test -f cat2dog.hfst$i ; then
        if ! echo "{cat}:{dog}" | ../../tools/src/hfst-calculate $FFLAG > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare cat2dog.hfst$i test.hfst  ; then
            exit 1
        fi
        rm test.hfst
    fi
done
