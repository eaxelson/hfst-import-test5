#!/bin/sh
if ! ../../scripts/hfst-lexc -o test.hfst cat.lexc ; then
    exit 1
fi
rm test.hfst
