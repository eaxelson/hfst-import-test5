#!/bin/bash
for f in ../src/hfst-* ; do
    if [ -x "$f" -a ! -d "$f" ] ; then
        $f < $srcdir/empty-file > /dev/null 2>&1
        rv=$?
        if [ $rv -eq 139 ] ; then
            echo $f received SEGV on empty input file
            exit 1
        elif [ $rv -gt 1 ] ; then
            echo $f did not die with SUCCESS nor FAILURE on empty input
            exit 1
        fi
    fi
done
