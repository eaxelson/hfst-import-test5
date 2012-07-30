#!/bin/sh
if ! test -x ../src/hfst-apertium-proc ; then
    echo apertium-proc missing
    exit 77
fi
if ! test -r $prefix/share/hfst/UnicodeData.txt ; then
    echo $prefix/share/hfst/UnicodeData.txt is missing, unicode handling may fail
fi
# basic lookup
if ! echo "cat" | ../src/hfst-apertium-proc cat2dog.hfst.ol > test.strings ; then
    echo cat fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-cat-out.strings ; then
    echo cat mismatched
    exit 1
fi

# basic generation (reverse-lookup)
if ! echo "^dog$" | ../src/hfst-apertium-proc -g cat2dog.gen.hfst.ol > test.strings ; then
    echo dog fail
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/cat.strings ; then
    echo generation mismatched
    exit 1
fi

# weighted lookup
if ! echo "cat" | ../src/hfst-apertium-proc -W cat_weight_final.hfst.ol > test.strings ; then
    echo heavy cat fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-cat-weighted-out.strings ; then
    echo heavy cat mismatched
    exit 1
fi

# capitalization checks
if ! ../src/hfst-apertium-proc proc-caps.hfst.ol < $srcdir/proc-caps-in.strings > test.strings ; then
    echo uppercase fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out1.strings ; then
    echo uppercase1 mismatched
    exit 1
fi
if ! ../src/hfst-apertium-proc -g proc-caps.gen.hfst.ol < $srcdir/proc-caps-gen.strings > test.strings ; then
    echo uppercase roundtrip fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out2.strings  ; then
    echo uppercase2 roundtrip mismatched
    exit 1
fi
if ! ../src/hfst-apertium-proc -c proc-caps.hfst.ol < $srcdir/proc-caps-in.strings > test.strings ; then
    echo uppercase fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out3.strings ; then
    echo uppercase3 mismatched
    exit 1
fi
if ! ../src/hfst-apertium-proc -w proc-caps.hfst.ol < $srcdir/proc-caps-in.strings > test.strings ; then
    echo uppercase4 fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out4.strings ; then
    echo uppercase4 mismatched
    exit 1
fi
if ! ../src/hfst-apertium-proc --cg --raw proc-caps.hfst.ol < $srcdir/proc-caps-in.strings > test.strings ; then
    echo raw cg fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out5.strings ; then
    echo raw cg mismatched
    exit 1
fi

if ! ../src/hfst-apertium-proc compounds.hfst.ol < $srcdir/proc-compounds.strings > test.strings ; then
    echo compound fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-compounds-out.strings ; then
    echo compound mismatched
    exit 1
fi
# from bug #3465122
if ! ../src/hfst-apertium-proc bashkir.hfst.ol < $srcdir/bashkir.strings > test.strings ; then
    echo bashkir fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-bashkir-out.strings ; then
    echo bashkir mismatched
    exit 1
fi
# from finntreebank boundary bug (priv)
if ! ../src/hfst-apertium-proc --cg proc-cg-compound-boundary-without-tags.hfst.ol < kaljakassi.strings > test.strings ; then
    echo kaljakassi fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-cg-compound-boundary-without-tags.strings ; then
    echo kaljakassi mismatched
    exit 1
fi
rm test.strings
