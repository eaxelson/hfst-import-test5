#!/bin/sh
comparables="cat.hfst dog.hfst tac.hfst cat2dog.hfst dog2cat.hfst cat_or_dog.hfst catdog.hfst"
for f in $comparables; do
    if ! ../../tools/src/hfst-compare -s $f $f  ; then
        echo hfst-compare -s mismatches $f $f
        exit 1
    fi
    for g in $comparables ; do
        if test $f != $g ; then
            if ../../tools/src/hfst-compare -s $f $g  ; then
                echo hfst-compare -s matches $f $g
                exit 1
            fi
        fi
    done
done

