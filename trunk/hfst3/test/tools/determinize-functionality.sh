#!/bin/sh
if ! ../../tools/src/hfst-determinize non_minimal.hfst > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst non_minimal.hfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
