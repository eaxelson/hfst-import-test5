#!/bin/bash

if test -x ./hfst-ospell ; then
    if ! cat $srcdir/test.strings | ./hfst-ospell speller.zhfst ; then
        exit 1
    fi
else
    exit 73
fi

