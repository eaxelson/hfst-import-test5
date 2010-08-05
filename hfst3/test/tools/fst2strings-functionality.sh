#!/bin/sh
if ! ../../tools/src/hfst-fst2strings < cat.hfst > test.strings ; then
    exit 1
fi
if ! diff test.strings cat.strings > /dev/null 2>&1 ; then
    exit 1
fi
rm test.strings
