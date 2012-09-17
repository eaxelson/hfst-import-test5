#!/bin/sh
TOOLDIR=../../tools/src
for i in "" .sfst .ofst .foma; do
    if test -f cat$i -a -f dog$i ; then
        if ! $TOOLDIR/hfst-substitute -s cat$i -F $srcdir/cat2dog.substitute > test ; then
            exit 1
        fi
        if ! $TOOLDIR/hfst-compare -s test dog$i  ; then
            exit 1
        fi
        rm test
        $TOOLDIR/hfst-substitute -s cat$i -f c -t d |\
            $TOOLDIR/hfst-substitute -s -f a -t o |\
            $TOOLDIR/hfst-substitute -s -f t -t g > test
        if test $? -ne 0 ; then 
            exit 1
        fi
        if ! $TOOLDIR/hfst-compare -s test dog$i ; then
            exit 1
        fi
    fi
done
