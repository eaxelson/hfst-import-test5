#!/bin/sh
TOOLDIR=../../tools/src

comparables="cat dog tac cat2dog dog2cat cat_or_dog catdog"
for f in $comparables; do
    if ! $TOOLDIR/hfst-compare -s $f.hfst $f.hfst  ; then
        echo "compare -s mismatches" $f.hfst $f.hfst
        exit 1
    fi
    for g in $comparables ; do
        if test $f != $g ; then
            if $TOOLDIR/hfst-compare -s $f.hfst $g.hfst  ; then
                echo "compare -s matches" $f.hfst $g.hfst
                exit 1
            fi
        fi
    done
done

