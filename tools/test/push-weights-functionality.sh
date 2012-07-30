#!/bin/sh
if ! ../src/hfst-push-weights -p initial cat2dog.hfst > test.hfst ; then
    exit 1
fi
if ! ../src/hfst-compare test.hfst cat2dog.hfst  ; then
    exit 1
fi
if ! ../src/hfst-push-weights -p final cat2dog.hfst > test.hfst ; then
    exit 1
fi
if ! ../src/hfst-compare test.hfst cat2dog.hfst  ; then
    exit 1
fi
rm test.hfst
