#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i ; then
        for c in 1 256 65536 4294967297 ; do
            if ! yes cat | head -n $c | ../../tools/src/hfst-proc cat2dog.hfst$i ; then
                exit 1
            fi
        done
    fi
done
