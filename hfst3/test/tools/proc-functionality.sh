#!/bin/sh

if [ "$srcdir" = "" ]; then
    srcdir="./";
fi

# basic lookup
if ! echo "cat" | ../../tools/src/hfst-proc/hfst-apertium-proc cat2dog.hfstol | tr -d '\r' > test.strings ; then
    echo cat fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-cat-out.strings ; then
    exit 1
fi

# basic generation (reverse-lookup)
if ! echo "^dog$" | ../../tools/src/hfst-proc/hfst-apertium-proc -g cat2dog.genhfstol | tr -d '\r' > test.strings ; then
    echo dog fail
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/cat.strings ; then
    exit 1
fi

# weighted lookup
if ! echo "cat" | ../../tools/src/hfst-proc/hfst-apertium-proc -W cat_weight_final.hfstol | tr -d '\r' > test.strings ; then
    echo heavy cat fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-cat-weighted-out.strings ; then
    exit 1
fi

# capitalization checks
if ! ../../tools/src/hfst-proc/hfst-apertium-proc proc-caps.hfstol < $srcdir/proc-caps-in.strings | tr -d '\r' > test.strings ; then
    echo uppercase fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out1.strings ; then
    exit 1
fi
if ! ../../tools/src/hfst-proc/hfst-apertium-proc -g proc-caps.genhfstol < $srcdir/proc-caps-gen.strings | tr -d '\r' > test.strings ; then
    echo uppercase roundtrip fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out2.strings  ; then
    exit 1
fi
if ! ../../tools/src/hfst-proc/hfst-apertium-proc -c proc-caps.hfstol < $srcdir/proc-caps-in.strings | tr -d '\r' > test.strings ; then
    echo uppercase fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out3.strings ; then
    exit 1
fi
if ! ../../tools/src/hfst-proc/hfst-apertium-proc -w proc-caps.hfstol < $srcdir/proc-caps-in.strings | tr -d '\r' > test.strings ; then
    echo uppercase fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out4.strings ; then
    exit 1
fi
if ! ../../tools/src/hfst-proc/hfst-apertium-proc --cg --raw proc-caps.hfstol < $srcdir/proc-caps-in.strings | tr -d '\r' > test.strings ; then
    echo raw cg fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-caps-out5.strings ; then
    exit 1
fi

if ! ../../tools/src/hfst-proc/hfst-apertium-proc compounds.hfstol < $srcdir/proc-compounds.strings | tr -d '\r' > test.strings ; then
    echo compound fail:
    cat test.strings
    exit 1
fi
if ! diff test.strings $srcdir/proc-compounds-out.strings ; then
    exit 1
fi
rm test.strings
