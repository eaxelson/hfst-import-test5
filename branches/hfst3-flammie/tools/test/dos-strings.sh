#!/bin/sh
../src/hfst-strings2fst $srcdir/dos.strings > test.hfst
RV=$?
rm test.hfst
exit $RV
