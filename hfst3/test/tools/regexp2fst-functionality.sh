#!/bin/sh
if ! ../../tools/src/hfst-regexp2fst < cats_and_dogs.xre > test.hfst ; then
    exit 1
fi
rm test.hfst
