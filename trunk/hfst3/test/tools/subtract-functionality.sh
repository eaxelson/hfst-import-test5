#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat_or_dog.hfst$i -a -f dog.hfst$i -a cat.hfst$i ; then
        if ! ../../tools/src/hfst-subtract cat_or_dog.hfst$i dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat.hfst$i > /dev/null 2>&1 ; then
            exit 1
        fi
        rm test.hfst;
    fi
done
if test -f cat2dog_0.3.hfst.ofst -a -f cat2dog_0.5.hfst.ofst ; then
    if ! ../../tools/src/hfst-subtract -1 cat2dog_0.3.hfst.ofst -2 cat2dog_0.5.hfst.ofst > test.hfst ; then
	exit 1;
    fi
    if ! ../../tools/src/hfst-compare empty.hfst.ofst test.hfst ; then
	exit 1;
    fi
    if ! ../../tools/src/hfst-subtract -2 cat2dog_0.3.hfst.ofst -1 cat2dog_0.5.hfst.ofst > test.hfst ; then
	exit 1;
    fi
    if ! ../../tools/src/hfst-compare empty.hfst.ofst test.hfst ; then
	exit 1;
    fi
    rm test.hfst;
fi