#!/bin/sh
if ! ../../tools/src/hfst-lookup cat.hfst < cat.strings > test.lookups ; then
    exit 1
fi
rm test.lookups
