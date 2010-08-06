#!/bin/sh
if ! ../../tools/src/hfst-substitute cat.hfst -F cat2dog.substitute > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst dog.hfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
