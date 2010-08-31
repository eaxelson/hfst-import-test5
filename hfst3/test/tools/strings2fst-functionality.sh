#!/bin/sh
if ! ../../tools/src/hfst-strings2fst < cat.strings > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
if ! ../../tools/src/hfst-strings2fst -S < c_a_t.strings > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst > /dev/null 2>&1 ; then
    exit 1
fi
