#!/bin/sh
if ! ../../tools/src/hfst-summarize cat.hfst > test.txt ; then
    exit 1
fi
rm test.txt
