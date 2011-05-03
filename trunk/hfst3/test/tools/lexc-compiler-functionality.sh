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
          stress.random-lexicons-100.lexc 
          xre.any-variations.lexc
          xre.automatic-multichar-symbols.lexc xre.basic.lexc 
          xre.definitions.lexc xre.months.lexc xre.nested-definitions.lexc 
          xre.numeric-star.lexc xre.sharp.lexc xre.star-plus-optional.lexc"
LEXCXFAIL="xfail.bogus.lexc xfail.ISO-8859-1.lexc xfail.lexicon-semicolon.lexc"
for i in "" .sfst .ofst .foma ; do
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
        if ! ../../tools/src/hfst-lexc2fst $FFLAG $srcdir/cat.lexc > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare cat.hfst$i test.hfst ; then
            exit 1
        fi
        rm test.hfst
    fi
    for f in $LEXCTESTS ; do
        echo DBG doing $FFLAG $f
        if ! ../../tools/src/hfst-lexc2fst $FFLAG $srcdir/$f > test.hfst ; then
            exit 1
        fi
        rm test.hfst
    done
done
