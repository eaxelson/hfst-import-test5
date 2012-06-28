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
    if test -f cat.hfst$i ; then
        if ! ../src/hfst-strings2fst $FFLAG $srcdir/cat.strings > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst cat.hfst$i ; then
            exit 1
        fi
        rm test.hfst
        if ! ../src/hfst-strings2fst $FFLAG -S $srcdir/c_a_t.strings > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst cat.hfst$i ; then
            exit 1
        fi
        rm test.hfst
    fi
    if test -f heavycat.hfst$i ; then
        if ! ../src/hfst-strings2fst $FFLAG $srcdir/heavycat.strings > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst heavycat.hfst$i ; then
            exit 1
        fi
        rm test.hfst
    fi
    if test -f cat2dog.hfst$i ; then
        if ! ../src/hfst-strings2fst $FFLAG $srcdir/cat2dog.strings > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst cat2dog.hfst$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../src/hfst-strings2fst $FFLAG -S $srcdir/cat2dog.spaces > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst cat2dog.hfst$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../src/hfst-strings2fst $FFLAG -p -S $srcdir/cat2dog.pairs > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst cat2dog.hfst$i > /dev/null 1>&1 ; then
            exit 1
        fi
        if ! ../src/hfst-strings2fst $FFLAG -p $srcdir/cat2dog.pairstring > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst cat2dog.hfst$i > /dev/null 1>&1 ; then
            exit 1
        fi
    fi
done
