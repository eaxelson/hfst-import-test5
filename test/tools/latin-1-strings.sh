#!/bin/sh
../../tools/src/hfst-strings2fst $srcdir/latin-1.strings > test.hfst
RV=$?
if test $RV -eq 0 ; then
    echo Latin-1 input incorrectly allowed
    exit 1
elif test $RV -gt 8 ; then
    exit 1
fi
rm test.hfst
