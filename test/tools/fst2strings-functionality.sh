#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i ; then
        if ! ../../tools/src/hfst-fst2strings cat.hfst$i > test.strings ; then
            echo turning cat.hfst$i to strings failed
            exit 1
        fi
        if ! diff test.strings $srcdir/cat.strings > /dev/null 2>&1 ; then
            echo cat.hfst$i strings differ from expected
            exit 1
        fi
        rm test.strings;
    fi
done

for i in "" .sfst .ofst; do
    if test -f empty.hfst$i ; then
	if ! ../../tools/src/hfst-fst2strings -r 20 empty.hfst$i > /dev/null ; then
	    echo "searching for random paths in an empty transducer failed"
	    exit 1
	fi
    fi
 done
