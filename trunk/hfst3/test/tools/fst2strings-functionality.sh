#!/bin/sh
TOOLDIR=../../tools/src
for i in "" .sfst .ofst .foma; do
    if test -f cat$i ; then
        if ! $TOOLDIR/hfst-fst2strings cat$i > test.strings ; then
            echo turning cat$i to strings failed
            exit 1
        fi
        if ! diff test.strings $srcdir/cat.strings > /dev/null 2>&1 ; then
            echo cat$i strings differ from expected
            exit 1
        fi
        rm test.strings;
    fi
done

for i in "" .sfst .ofst; do
    if test -f empty$i ; then
	if ! $TOOLDIR/hfst-fst2strings -r 20 empty$i > /dev/null ; then
	    echo "searching for random paths in an empty transducer failed"
	    exit 1
	fi
    fi
 done
