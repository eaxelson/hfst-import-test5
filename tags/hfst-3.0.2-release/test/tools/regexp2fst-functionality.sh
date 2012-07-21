#!/bin/sh
for i in sfst openfst-tropical foma; do
    if ! ../../tools/src/hfst-regexp2fst -f $i < cats_and_dogs.xre > test.hfst ; then
        exit 1
    fi
    rm test.hfst;
    if ! ../../tools/src/hfst-regexp2fst -S -f $i < cats_and_dogs_semicolon.xre > test.hfst ; then
        exit 1
    fi
    rm test.hfst;
done