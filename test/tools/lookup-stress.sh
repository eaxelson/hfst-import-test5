#!/bin/sh
for i in "" ol .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i ; then
        for c in 1 256 65536 16777216 ; do
            echo doing $c times cat in $i
            if ! yes cat | head -n $c | ../../tools/src/hfst-lookup cat2dog.hfst$i > /dev/null ; then
                echo "died with $? ($PIPESTATUS)"
                exit 1
            fi
        done
    fi
done

