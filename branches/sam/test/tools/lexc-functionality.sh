#!/bin/sh
LEXCTESTS="basic.cat-dog-bird.lexc basic.colons.lexc basic.comments.lexc 
          basic.empty-sides.lexc basic.end.lexc basic.escapes.lexc 
          basic.infostrings.lexc basic.initial-lexicon-empty.lexc 
          basic.multichar-symbols.lexc basic.multichar-symbol-with-0.lexc 
          basic.multi-entry-lines.lexc basic.no-newline-at-end.lexc 
          basic.no-Root.lexc basic.punctuation.lexc basic.root-loop.lexc 
          basic.spurious-lexicon.lexc basic.string-pairs.lexc 
          basic.two-lexicons.lexc basic.UTF-8.lexc basic.zeros-epsilons.lexc 
          hfst.weights.lexc 
          xre.any-variations.lexc
          xre.automatic-multichar-symbols.lexc xre.basic.lexc 
          xre.definitions.lexc xre.months.lexc xre.nested-definitions.lexc 
          xre.numeric-star.lexc xre.sharp.lexc xre.quotations.lexc
          xre.star-plus-optional.lexc"
LEXCXFAIL="xfail.bogus.lexc xfail.ISO-8859-1.lexc xfail.lexicon-semicolon.lexc"

if ! test -x ../../tools/src/hfst-lexc ; then
    echo hfst-lexc not executable, assumed configured off and skipping
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

    if ! (../../tools/src/hfst-format --test-format $FNAME ) ; then
        continue;
    fi

    if test -f cat$i ; then
        if ! ../../tools/src/hfst-lexc $FFLAG $srcdir/cat.lexc -o test ; then
            echo lexc2fst $FFLAG cat.lexc failed with $?
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s cat$i test ; then
            exit 1
        fi
        rm test
    fi
    for f in $LEXCTESTS ; do
        if ! ../../tools/src/hfst-lexc $FFLAG $srcdir/$f -o test ; then
            echo lexc2fst $FFLAG $f failed with $?
            exit 1
        fi
        rm test
    done
    if ! ../../tools/src/hfst-lexc $FFLAG $srcdir/basic.multi-file-1.lexc \
        $srcdir/basic.multi-file-2.lexc \
        $srcdir/basic.multi-file-3.lexc -o test ; then
        echo lexc2fst $FFLAG basic.multi-file-{1,2,3}.lexc failed with $?
        exit 1
    fi
    if ! ../../tools/src/hfst-compare -s walk_or_dog$i test ; then
        exit 1
    fi
done
