#!/bin/sh

if ! test -x ../../tools/src/hfst-lexc; then
    exit 0;
fi

if ! ../../tools/src/hfst-lexc -o test.hfst $srcdir/cat.lexc ; then
    exit 1
fi
rm test.hfst
