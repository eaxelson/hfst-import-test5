#!/bin/sh
for f in *.hfst ; do
    if ! ../../tools/src/hfst-compare $f $f > /dev/null 2>&1 ; then
        echo hfst-compare mismatches $f $f
        exit 1
    fi
    for g in *.hfst ; do
        if test $f != $g ; then
            if hfst-compare $f $g > /dev/null 2>&1 ; then
                echo hfst-compare matches $f $g
                exit 1
            fi
        fi
    done
done

