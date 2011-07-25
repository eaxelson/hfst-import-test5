#!/bin/sh
../../tools/src/hfst-strings2fst $srcdir/latin-1.strings > test.hfst
RV=$?
rm test.hfst
exit $RV
