#!/bin/sh
TOOLDIR=../../tools/src

EXT=
if (uname | egrep "MINGW|mingw" 2>1 > /dev/null); then
    EXT=".exe";
fi

for f in $TOOLDIR/hfst-* ; do
    # Skip deprecated tools that can still be installed somewhere
    if [ "$f" != "$TOOLDIR/hfst-xfst2fst""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-lexc-compiler""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-twolc-system""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-duplicate""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-preprocess-for-optimized-lookup-format""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-strip-header""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-train-tagger-system""$EXT" ]; then
        if [ -x "$f" -a ! -d "$f" ] ; then
            if ! "$f" --help > help.out ; then
                rm help.out
                echo $f has broken help
                exit 1
            fi
            if ! grep -q '^Usage: hfst-' help.out > /dev/null ; then
                rm help.out
                echo $f has malformed help
                exit 1
            fi
        fi
    fi
done
rm help.out

