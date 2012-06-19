#!/bin/sh
if ! ../../tools/src/hfst-strings2fst $srcdir/utf-8.strings > test ; then
    exit 1
fi
if ! ../../tools/src/hfst-fst2strings < test > test.strings ; then
    exit 1
fi
rm test test.strings
