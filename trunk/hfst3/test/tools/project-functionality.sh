#!/bin/sh
if ! ../../tools/src/hfst-project -p upper cat2dog.hfst > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst dog.hfst > /dev/null 2>&1 ; then
    exit 1
fi
if ! ../../tools/src/hfst-project -p lower cat2dog.hfst > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
