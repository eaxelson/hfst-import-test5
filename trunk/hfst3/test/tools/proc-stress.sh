#!/bin/sh
if test -f cat2dog.hfstol ; then
    for c in 1 256 65536 16777216 ; do
        if ! yes cat | head -n $c | ../../tools/src/hfst-proc/hfst-apertium-proc cat2dog.hfstol > /dev/null ; then
            exit 1
        fi
    done
fi
