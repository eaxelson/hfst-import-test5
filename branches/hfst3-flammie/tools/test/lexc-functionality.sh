#!/bin/sh

if ! test -x ../src/hfst-lexc; then
    exit 0;
fi

if ! ../src/hfst-lexc -o test.hfst $srcdir/cat.lexc ; then
    exit 1
fi
rm test.hfst
