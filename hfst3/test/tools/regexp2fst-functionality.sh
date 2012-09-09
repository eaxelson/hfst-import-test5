#!/bin/sh
#if [ "$srcdir" = "" ] ; then
#    srcdir=. ;
#fi

for i in sfst openfst-tropical foma; do
    if ! (../../tools/src/hfst-format --list-formats | grep $i > /dev/null) ; then
	continue;
    fi
    if ! ../../tools/src/hfst-regexp2fst -f $i $srcdir/cats_and_dogs.xre > test ; then
        exit 1
    fi
    rm test;
    if ! ../../tools/src/hfst-regexp2fst -S -f $i $srcdir/cats_and_dogs_semicolon.xre > test ; then
        exit 1
    fi
    rm test;
    if ! ../../tools/src/hfst-regexp2fst -f $i $srcdir/at_file_quote.$i.xre > test.fst ; then
        exit 1
    fi
    if ! ../../tools/src/hfst-regexp2fst -f $i $srcdir/not-contains-a.xre > test.fst ; then
        exit 1
    fi
    if ! ../../tools/src/hfst-regexp2fst -S -f $i $srcdir/not-contains-a-comment-emptyline.xre > test.fst ; then
        exit 1
    fi
done

rm -f test.fst
