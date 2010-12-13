#!/bin/sh
if ! ../../tools/src/hfst-lexc-compiler -f sfst < cat.lexc > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-lexc-compiler -f openfst-tropical < cat.lexc > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-lexc-compiler -f foma < cat.lexc > test.hfst ; then
    exit 1
fi
rm test.hfst
