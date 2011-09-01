#!/bin/sh
if ! ../../tools/src/hfst-lexc -o test.hfst $srcdir/cat.lexc ; then
    exit 1
fi
rm test.hfst
