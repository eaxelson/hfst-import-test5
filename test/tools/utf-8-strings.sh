#!/bin/sh
if ! ../../tools/src/hfst-strings2fst < utf-8.strings > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-fst2strings < test.hfst > test.strings ; then
    exit 1
fi
rm test.hfst test.strings
