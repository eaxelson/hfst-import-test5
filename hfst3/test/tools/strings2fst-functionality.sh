#!/bin/sh
if ! ../../tools/src/hfst-strings2fst -f sfst < cat.strings > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst.sfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
if ! ../../tools/src/hfst-strings2fst -f sfst -S < c_a_t.strings > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst.sfst > /dev/null 2>&1 ; then
    exit 1
fi

if ! ../../tools/src/hfst-strings2fst -f openfst-tropical < cat.strings > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst.ofst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
if ! ../../tools/src/hfst-strings2fst -f openfst-tropical -S < c_a_t.strings > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst.ofst > /dev/null 2>&1 ; then
    exit 1
fi

if ! ../../tools/src/hfst-strings2fst -f foma < cat.strings > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst.foma > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
if ! ../../tools/src/hfst-strings2fst -f foma -S < c_a_t.strings > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst.foma > /dev/null 2>&1 ; then
    exit 1
fi
