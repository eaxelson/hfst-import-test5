#!/bin/sh
if ! ../../tools/src/hfst-push-weights -p initial cat2dog > test ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare -s test cat2dog  ; then
    exit 1
fi
if ! ../../tools/src/hfst-push-weights -p final cat2dog > test ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare -s test cat2dog  ; then
    exit 1
fi
rm test
