#!/bin/sh
../../tools/src/hfst-strings2fst $srcdir/latin-1.strings > test.hfst \
    2> /dev/null
RV=$?
rm test.hfst
exit $RV
