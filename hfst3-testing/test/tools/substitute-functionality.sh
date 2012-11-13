#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat$i -a -f dog$i ; then
        if ! ../../tools/src/hfst-substitute -s cat$i -F $srcdir/cat2dog.substitute > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test dog$i  ; then
            exit 1
        fi
        rm test
        ../../tools/src/hfst-substitute -s cat$i -f c -t d |\
            ../../tools/src/hfst-substitute -s -f a -t o |\
            ../../tools/src/hfst-substitute -s -f t -t g > test
        if test $? -ne 0 ; then 
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test dog$i ; then
            exit 1
        fi
    fi
done
