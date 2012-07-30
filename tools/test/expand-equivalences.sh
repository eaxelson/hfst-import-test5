#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test x$i = x.foma ; then
        echo "FOMA is known to be broken here"
    elif test -f cat.hfst$i -a -f dog.hfst$i ; then
        if ! ../src/hfst-expand-equivalences cat.hfst$i -T $srcdir/cat2dog.expand > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst cat_or_dog2cat.hfst$i  ; then
            exit 1
        fi
        rm test.hfst
        ../src/hfst-expand-equivalences cat.hfst$i -f c -t d |\
            ../src/hfst-expand-equivalences -f a -t o |\
            ../src/hfst-expand-equivalences -f t -t g > test.hfst
        if test $? -ne 0 ; then 
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst cat_or_dog2cat.hfst$i ; then
            exit 1
        fi
        if ! ../src/hfst-expand-equivalences romanian.hfst$i -a $srcdir/apertium-es-ro.ro.acx > test.hfst ; then
            exit 1
        fi
    fi
done
