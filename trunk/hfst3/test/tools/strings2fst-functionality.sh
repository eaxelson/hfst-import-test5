#!/bin/sh
for i in "" .sfst .ofst .foma; do
    FFLAG=
    case $i in
        .sfst)
            FFLAG="-f sfst";;
        .ofst)
            FFLAG="-f openfst-tropical";;
        .foma)
            FFLAG="-f foma";;
        *)
            FFLAG=;;
    esac
    if test -f cat$i ; then
        if ! ../../tools/src/hfst-strings2fst $FFLAG $srcdir/cat.strings > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat$i ; then
            exit 1
        fi
        rm test
        if ! ../../tools/src/hfst-strings2fst $FFLAG -S $srcdir/c_a_t.strings > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat$i ; then
            exit 1
        fi
        rm test
    fi
    if test -f heavycat$i ; then
        if ! ../../tools/src/hfst-strings2fst $FFLAG $srcdir/heavycat.strings > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test heavycat$i ; then
            exit 1
        fi
        rm test
    fi
    if test -f cat2dog$i ; then
        if ! ../../tools/src/hfst-strings2fst $FFLAG $srcdir/cat2dog.strings > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat2dog$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-strings2fst $FFLAG -S $srcdir/cat2dog.spaces > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat2dog$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-strings2fst $FFLAG -p -S $srcdir/cat2dog.pairs > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat2dog$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-strings2fst $FFLAG -p $srcdir/cat2dog.pairstring > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat2dog$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-strings2fst $srcdir/dos.strings > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test.hfst cat$i ; then
            exit 1
        fi
        rm test.hfst
    fi
done

