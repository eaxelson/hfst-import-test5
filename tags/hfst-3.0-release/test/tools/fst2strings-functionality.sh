#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i ; then
        if ! ../../tools/src/hfst-fst2strings < cat.hfst$i > test.strings ; then
            echo turning cat.hfst$i to strings failed
            exit 1
        fi
        if ! diff test.strings cat.two-level.strings > /dev/null 2>&1 ; then
            echo cat.hfst$i strings differ from expected
            exit 1
        fi
        rm test.strings;
    fi
done
