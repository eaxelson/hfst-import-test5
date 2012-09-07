#!/bin/sh

EXT=
if ! (uname | egrep "Linux|linux" 2>1 > /dev/null); then
    EXT=".exe";
fi

for f in ../../tools/src/hfst-* ; do
    if [ "$f" != "../../tools/src/hfst-lexc2fst""$EXT" -a \
        "$f" != "../../tools/src/hfst-xfst2fst""$EXT" -a \
	"$f" != "../../tools/src/hfst-lexc-compiler""$EXT" -a \
	"$f" != "../../tools/src/hfst-info""$EXT" ]; then
        if [ -x "$f" -a ! -d "$f" ] ; then
            if ! "$f" --help > help.out ; then
                rm help.out
                echo $f has broken help
                exit 1
            fi
            if ! grep -m 1 '^Usage: hfst-' help.out > /dev/null ; then
                rm help.out
                echo $f has malformed help
                exit 1
            fi
        fi
    fi
done
rm help.out

