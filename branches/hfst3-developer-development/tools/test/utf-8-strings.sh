#!/bin/sh
if ! ../src/hfst-strings2fst $srcdir/utf-8.strings > test.hfst ; then
    exit 1
fi
if ! ../src/hfst-fst2strings < test.hfst > test.strings ; then
    exit 1
fi
rm test.hfst test.strings