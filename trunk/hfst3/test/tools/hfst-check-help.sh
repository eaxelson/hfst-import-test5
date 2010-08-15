#!/bin/sh
for f in ../../tools/src/hfst-* ; do
    if [ -x "$f" -a ! -d "$f" ] ; then
        if ! "$f" --help > help.out ; then
            rm help.out
            echo $f has broken help
            exit 1
        fi
        if ! grep -m 1 '^Usage: .*hfst-' help.out > /dev/null ; then
            rm help.out
            echo $f has malformed help
            exit 1
        fi
    fi
done
rm help.out

