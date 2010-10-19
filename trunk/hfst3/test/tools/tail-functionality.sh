#!/bin/sh
if ! cat cat2dog.hfst dog2cat.hfst | ../../tools/src/hfst-tail -n 1 > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst dog2cat.hfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
