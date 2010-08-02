#!/bin/sh
for f in *.hfst ; do
    if ! ../../tools/src/hfst-compare "$f" "$f" > /dev/null 2>&1 ; then
        echo hfst-compare mismatches $f $f
        exit $?
    fi
done

