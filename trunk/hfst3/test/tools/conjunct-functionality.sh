#!/bin/sh
if ! ../../tools/src/hfst-conjunct cat_or_dog.hfst cat.hfst > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst cat.hfst > /dev/null 2>&1 ; then
    exit 1
fi
rm test.hfst
