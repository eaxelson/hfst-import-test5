#!/bin/sh
comparables="cat.hfst dog.hfst tac.hfst cat2dog.hfst dog2cat.hfst cat_or_dog.hfst catdog.hfst"
for f in $comparables; do
    if ! ../src/hfst-compare $f $f  ; then
        echo hfst-compare mismatches $f $f
        exit 1
    fi
    for g in $comparables ; do
        if test $f != $g ; then
            if ../src/hfst-compare $f $g  ; then
                echo hfst-compare matches $f $g
                exit 1
            fi
        fi
    done
done
