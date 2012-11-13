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
        if ! echo "cat|dog" | ../../tools/src/hfst-calculate $FFLAG > lexicon ; then
            exit 1
        fi
        if ! echo "([catdog]|d:D|c:C)*" | ../../tools/src/hfst-calculate $FFLAG > rules ; then
            exit 1
        fi
        if ! echo "([catdog]|d:D|c:C)*" | ../../tools/src/hfst-calculate $FFLAG >> rules ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compose-intersect -1 lexicon -2 rules > test ; then
            exit 1
        fi
        if ! echo "cat|dog|{cat}:{Cat}|{dog}:{Dog}" | ../../tools/src/hfst-calculate $FFLAG > test2 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test test2  ; then
            exit 1
        fi
        rm test test2 lexicon rules
    fi
done
