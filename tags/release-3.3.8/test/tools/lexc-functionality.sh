#!/bin/sh

if ! test -x ../../tools/src/hfst-lexc; then
    exit 0;
fi

if ! ../../tools/src/hfst-lexc -s -o test $srcdir/cat.lexc \
    > /dev/null; then
    exit 1
fi
rm test
