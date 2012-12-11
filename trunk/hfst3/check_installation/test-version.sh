#!/bin/sh
TOOLSDIR=`which hfst-repeat | sed 's/hfst-repeat//'`

EXT=
if (uname | egrep "MINGW|mingw" 2>1 > /dev/null); then
    EXT=".exe";
fi

for f in $TOOLSDIR/hfst-* ; do
    if [ "$f" != "hfst-lexc""$EXT" -a \
         "$f" != "hfst-lexc2fst""$EXT" -a \
         "$f" != "hfst-xfst2fst""$EXT" -a \
	 "$f" != "hfst-lexc-compiler""$EXT" -a \
	 "$f" != "hfst-pmatch""$EXT" -a \
	 "$f" != "hfst-preprocess-for-optimized-lookup-format""$EXT" -a \
	 "$f" != "hfst-duplicate""$EXT" -a \
	 "$f" != "hfst-strip-header""$EXT" -a \
	 "$f" != "hfst-info""$EXT" ] ; then
        if [ -x "$f" -a ! -d "$f" ] ; then
            if ! "$f" --version > version.out ; then
                rm version.out
                echo $f has broken version
                exit 1
            fi
            if ! grep -q 'hfst-[^ ]\+ [0-9.]\+' version.out > /dev/null \
            ; then
                rm version.out
                echo $f has malformed version
                exit 1
            fi
        fi
    fi
done
rm version.out

