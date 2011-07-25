#!/bin/sh
for f in "" .sfst .foma .openfst; do
    for g in "" .sfst .foma .openfst; do
        if test cat.hfst$f != cat.hfst$g ; then
            if test -f cat.hfst$f -a -f cat.hfst$g ; then
                if ../../tools/src/hfst-compare cat.hfst$f cat.hfst$g ; then
                    echo hfst-compare does not fail cat.hfst$f cat.hfst$g
                    exit 0
                fi
            fi
        fi
    done
done
exit 2
