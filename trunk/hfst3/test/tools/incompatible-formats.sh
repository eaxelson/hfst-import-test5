#!/bin/sh
for f in "" .sfst .foma .openfst; do
    for g in "" .sfst .foma .openfst; do
        if test cat.hfst$f != cat.hfst$g ; then
            if test -f cat.hfst$f -a -f cat.hfst$g ; then
                for p in compare compose concatenate conjunct disjunct subtract ; do
                    if ../../tools/src/hfst-$p cat.hfst$f cat.hfst$g ; then
                        echo hfst-$p does not fail cat.hfst$f cat.hfst$g
                        exit 0
                    fi
                done
            fi
        fi
    done
done
exit 2
