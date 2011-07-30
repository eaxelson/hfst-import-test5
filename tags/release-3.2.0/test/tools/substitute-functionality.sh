#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i -a -f dog.hfst$i ; then
        if ! ../../tools/src/hfst-substitute cat.hfst$i -F $srcdir/cat2dog.substitute > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst dog.hfst$i  ; then
            exit 1
        fi
        rm test.hfst
        ../../tools/src/hfst-substitute cat.hfst$i -f c -t d |\
            ../../tools/src/hfst-substitute -f a -t o |\
            ../../tools/src/hfst-substitute -f t -t g > test.hfst
        if test $? -ne 0 ; then 
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst dog.hfst$i ; then
            exit 1
        fi
    fi
done
