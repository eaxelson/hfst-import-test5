#!/bin/sh
if ! cat cat2dog.hfst dog2cat.hfst | ../../tools/src/hfst-split ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare 1.hfst cat2dog.hfst > /dev/null 2>&1 ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare 2.hfst dog2cat.hfst > /dev/null 2>&1 ; then
    exit 1
fi
rm 1.hfst 2.hfst
