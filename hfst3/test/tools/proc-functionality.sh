#!/bin/sh

# basic lookup
if ! echo "cat" | ../../tools/src/hfst-proc/hfst-proc cat2dog.hfst.ol > test.strings ; then
    echo cat fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-cat-out.strings ; then
    exit 1
fi

# basic generation (reverse-lookup)
if ! echo "^dog$" | ../../tools/src/hfst-proc/hfst-proc -g cat2dog.gen.hfst.ol > test.strings ; then
    echo dog fail
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/cat.strings ; then
    exit 1
fi

# weighted lookup
if ! echo "cat" | ../../tools/src/hfst-proc/hfst-proc -W cat_weight_final.hfst.ol > test.strings ; then
    echo heavy cat fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-cat-weighted-out.strings ; then
    exit 1
fi

# capitalization checks
if ! ../../tools/src/hfst-proc/hfst-proc proc-caps.hfst.ol < $srcdir/proc-caps-in.strings > test.strings ; then
    echo uppercase fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out1.strings ; then
    exit 1
fi
if ! hfst-proc -g proc-caps.gen.hfst.ol < $srcdir/proc-caps-gen.strings > test.strings ; then
    echo uppercase roundtrip fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out2.strings  ; then
    exit 1
fi
if ! ../../tools/src/hfst-proc/hfst-proc -c proc-caps.hfst.ol < $srcdir/proc-caps-in.strings > test.strings ; then
    echo uppercase fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out3.strings ; then
    exit 1
fi
if ! ../../tools/src/hfst-proc/hfst-proc -w proc-caps.hfst.ol < $srcdir/proc-caps-in.strings > test.strings ; then
    echo uppercase fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out4.strings ; then
    exit 1
fi
if ! ../../tools/src/hfst-proc/hfst-proc --cg --raw proc-caps.hfst.ol < $srcdir/proc-caps-in.strings > test.strings ; then
    echo raw cg fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out5.strings ; then
    exit 1
fi

if ! ../../tools/src/hfst-proc/hfst-proc compounds.hfst.ol < $srcdir/proc-compounds.strings > test.strings ; then
    echo compound fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-compounds-out.strings ; then
    exit 1
fi
rm test.strings
