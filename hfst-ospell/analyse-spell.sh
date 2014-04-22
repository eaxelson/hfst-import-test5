#!/bin/bash

if test -x ./hfst-ospell ; then
    if ! cat $srcdir/test.strings | ./hfst-ospell -a speller_analyser.zhfst ; then
        exit 1
    fi
else
    echo ./hfst-ospell not built
    exit 77
fi

