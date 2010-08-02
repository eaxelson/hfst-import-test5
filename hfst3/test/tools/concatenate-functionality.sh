#!/bin/sh
if ! ../../tools/src/hfst-concatenate cat.hfst dog.hfst > test.hfst ; then
    exit $?
fi
if ! ../../tools/src/hfst-compare test.hfst catdog.hfst ; then
    exit $?
fi
rm test.hfst
