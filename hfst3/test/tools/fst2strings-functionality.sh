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
    # extract 100 times 5 random strings and check that flags are obeyed
    # disabled until flags work..
#    if test -f unification_flags$i ; then
#        for foo in 0 1 2 3 4 5 6 7 8 9
#        do
#            for bar in 0 1 2 3 4 5 6 7 8 9
#            do
#                if ! $TOOLDIR/hfst-fst2strings --random 5 -X obey-flags \
#                    unification_flags$i > test.strings ; then
#                    echo extracting random flags from unification_flags$i failed
#                    exit 1
#                fi
#            # flags do not allow [A|B|C] with [a|b|c]
#                if (egrep "A|B|C" test.strings | egrep "a|b|c" > /dev/null); then
#                    echo "error in processing flags in "unification_flags$i
#                    exit 1
#                fi
#            done
#        done
#    fi
done

for i in "" .sfst .ofst; do
    if test -f empty$i ; then
	if ! $TOOLDIR/hfst-fst2strings -r 20 empty$i > /dev/null ; then
	    echo "searching for random paths in an empty transducer failed"
	    exit 1
	fi
    fi
 done
