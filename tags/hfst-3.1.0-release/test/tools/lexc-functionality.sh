#!/bin/sh
PATH=$srcdir/../../scripts:../../scripts/:$PATH
if ! $srcdir/../../scripts/hfst-lexc -o test.hfst $srcdir/cat.lexc ; then
    exit 1
fi
rm test.hfst