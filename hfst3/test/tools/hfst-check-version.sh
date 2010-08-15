#!/bin/sh
for f in ../../tools/src/hfst-* ; do
    if [ -x "$f" -a ! -d "$f" ] ; then
        if ! "$f" --version > version.out ; then
            rm version.out
            echo $f has broken version
            exit 1
        fi
        if ! grep -m 1 '^hfst-[^ ]\+ [0-9.]\+' version.out > /dev/null ; then
            rm version.out
            echo $f has malformed version
            exit 1
        fi
    fi
done
rm version.out

