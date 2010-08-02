#!/bin/sh
for f in ../../tools/src/hfst-* ; do
    if [ -x "$f" ] ; then
        $f < empty-file > /dev/null 2>&1
        if [ $? == 139 ] ; then
            echo $f received SEGV on empty input file
            exit 1
        elif [ $? -gt 11 ] ; then
            echo $f did not die with SUCCESS nor FAILURE on empty input
            exit 1
        fi
    fi
done

