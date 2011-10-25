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
          xre.numeric-star.lexc xre.sharp.lexc xre.star-plus-optional.lexc"
LEXCXFAIL="xfail.bogus.lexc xfail.ISO-8859-1.lexc xfail.lexicon-semicolon.lexc"
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

    if test -f cat.hfst$i ; then
        if ! ../../tools/src/hfst-lexc2fst $FFLAG cat.lexc > test.hfst ; then
            echo lexc2fst $FFLAG cat.lexc failed with $?
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s cat.hfst$i test.hfst ; then
            exit 1
        fi
        rm test.hfst
    fi
    for f in $LEXCTESTS ; do
        if ! ../../tools/src/hfst-lexc2fst $FFLAG $f > test.hfst ; then
            echo lexc2fst $FFLAG $f failed with $?
            exit 1
        fi
        rm test.hfst
    done
    if ! ../../tools/src/hfst-lexc2fst $FFLAG basic.multi-file-1.lexc \
        basic.multi-file-2.lexc \
        basic.multi-file-3.lexc > test.hfst ; then
        echo lexc2fst $FFLAG basic.multi-file-{1,2,3}.lexc failed with $?
        exit 1
    fi
    if ! ../../tools/src/hfst-compare -s walk_or_dog.hfst$i test.hfst ; then
        exit 1
    fi
done
