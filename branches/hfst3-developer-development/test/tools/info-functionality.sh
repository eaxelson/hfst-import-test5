#!/bin/sh
if ! ../../tools/src/hfst-info -v > test.txt ; then
    echo "info verbose printout failed"
    exit 1
fi
rm -f test.txt
if ! ../../tools/src/hfst-info -a 3 ; then
    echo "version test >3 failed"
    exit 1
fi
if ! ../../tools/src/hfst-info -a 3.2 ; then
    echo "version test >3.2 failed"
    exit 1
fi
