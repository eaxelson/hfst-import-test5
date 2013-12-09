#!/bin/sh

# skip tests
# exit 77

TOOLDIR=../../tools/src
LEXCTESTS="basic.cat-dog-bird.lexc basic.colons.lexc basic.comments.lexc 
          basic.empty-sides.lexc basic.end.lexc basic.escapes.lexc 
          basic.infostrings.lexc basic.initial-lexicon-empty.lexc 
          basic.multichar-symbols.lexc basic.multichar-symbol-with-0.lexc 
          basic.multi-entry-lines.lexc basic.no-newline-at-end.lexc 
          basic.no-Root.lexc basic.punctuation.lexc basic.root-loop.lexc 
          basic.spurious-lexicon.lexc basic.string-pairs.lexc 
          basic.two-lexicons.lexc basic.UTF-8.lexc basic.zeros-epsilons.lexc
         basic.lowercase-lexicon-end.lexc basic.multichar-flag-with-zero.lexc 
          hfst.weights.lexc 
          xre.any-variations.lexc
          xre.automatic-multichar-symbols.lexc xre.basic.lexc 
          xre.definitions.lexc xre.months.lexc xre.nested-definitions.lexc 
          xre.numeric-star.lexc xre.sharp.lexc xre.quotations.lexc
          xre.star-plus-optional.lexc"
LEXCXFAIL="xfail.bogus.lexc xfail.ISO-8859-1.lexc xfail.lexicon-semicolon.lexc"

if test "$srcdir" = ""; then
    srcdir="./"
fi


if ! test -x $TOOLDIR/hfst-lexc2fst ; then
    echo "missing hfst-lexc2fst, assuming configured off, skipping"
    exit 73
fi

for i in .sfst .ofst .foma ; do
    FFLAG=
    FNAME=
    case $i in
        .sfst)
            FNAME="sfst";
            FFLAG="-f sfst";;
        .ofst)
            FNAME="openfst-tropical";
            FFLAG="-f openfst-tropical";;
        .foma)
            FNAME="foma";
            FFLAG="-f foma";;
        *)
            FNAME=;
            FFLAG=;;
    esac

    if ! ($TOOLDIR/hfst-format --test-format $FNAME ) ; then
        continue;
    fi

    if test -f cat$i ; then
        if ! $TOOLDIR/hfst-lexc2fst $FFLAG $srcdir/cat.lexc > test 2> /dev/null; then
            echo lexc2fst $FFLAG cat.lexc failed with $?
            exit 1
        fi
        if ! $TOOLDIR/hfst-compare -e -s cat$i test ; then
	    echo "results differ: " "cat"$i" test"
            exit 1
        fi
        rm test
    fi
    for f in $LEXCTESTS ; do
        if ! $TOOLDIR/hfst-lexc2fst $FFLAG $srcdir/$f > test 2> /dev/null; then
            echo lexc2fst $FFLAG $f failed with $?
            exit 1
        fi
        rm test
    done
    if ! $TOOLDIR/hfst-lexc2fst $FFLAG $srcdir/basic.multi-file-1.lexc \
        $srcdir/basic.multi-file-2.lexc \
        $srcdir/basic.multi-file-3.lexc > test 2> /dev/null; then
        echo lexc2fst $FFLAG basic.multi-file-{1,2,3}.lexc failed with $?
        exit 1
    fi
    if ! $TOOLDIR/hfst-compare -e -s walk_or_dog$i test ; then
        exit 1
    fi
done
