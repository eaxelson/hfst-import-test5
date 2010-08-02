#!/bin/sh
if ! ../../tools/src/hfst-compose cat.hfst cat2dog.hfst > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat2dog.hfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
