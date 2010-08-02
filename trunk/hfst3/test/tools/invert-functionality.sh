#!/bin/sh
if ! ../../tools/src/hfst-invert cat2dog.hfst > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst dog2cat.hfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
