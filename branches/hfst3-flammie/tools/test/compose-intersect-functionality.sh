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
        if ! echo "cat|dog" | ../src/hfst-calculate $FFLAG > lexicon.hfst ; then
            exit 1
        fi
        if ! echo "([catdog]|d:D|c:C)*" | ../src/hfst-calculate $FFLAG > rules.hfst ; then
            exit 1
        fi
        if ! echo "([catdog]|d:D|c:C)*" | ../src/hfst-calculate $FFLAG >> rules.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compose-intersect -1 lexicon.hfst -2 rules.hfst > test.hfst ; then
            exit 1
        fi
        if ! echo "cat|dog|{cat}:{Cat}|{dog}:{Dog}" | ../src/hfst-calculate $FFLAG > test2.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst test2.hfst  ; then
            exit 1
        fi
        rm test.hfst test2.hfst lexicon.hfst rules.hfst
    fi
done