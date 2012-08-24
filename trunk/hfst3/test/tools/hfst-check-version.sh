#!/bin/sh
for f in ../../tools/src/hfst-* ; do
    if [ "$f" != "../../tools/src/hfst-lexc" -a \
         "$f" != "../../tools/src/hfst-lexc2fst" -a \
         "$f" != "../../tools/src/hfst-xfst2fst" -a \
	 "$f" != "../../tools/src/hfst-lexc-compiler" -a \
	 "$f" != "../../tools/src/hfst-info" ] ; then
        if [ -x "$f" -a ! -d "$f" ] ; then
            if ! "$f" --version > version.out ; then
                rm version.out
                echo $f has broken version
                exit 1
            fi
            if ! grep -m 1 'hfst-[^ ]\+ [0-9.]\+' version.out > /dev/null \
            ; then
                rm version.out
                echo $f has malformed version
                exit 1
            fi
        fi
    fi
done
rm version.out

