#!/bin/sh
TOOLDIR=../../tools/src
if ! $TOOLDIR/hfst-push-weights -p initial cat2dog > test ; then
    exit 1
fi
if ! $TOOLDIR/hfst-compare -s test cat2dog  ; then
    exit 1
fi
if ! $TOOLDIR/hfst-push-weights -p final cat2dog > test ; then
    exit 1
fi
if ! $TOOLDIR/hfst-compare -s test cat2dog  ; then
    exit 1
fi
rm test
