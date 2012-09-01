#!/bin/bash
old_opts="$HFST_OPTIONS"
export HFST_OPTIONS="--version"
for f in ../src/hfst-{conjunct,disjunct,compose,subtract,compose,compose-intersect} ; do
    if [ -x "$f" -a ! -d "$f" ] ; then
        if !  "$f" -1 $srcdir/cat.hfst -2 $srcdir/dog.hfst -o /dev/null > version.out ; then
            rm version.out
            echo "$f has broken version (when asking from env. var)"
            exit 1
        fi
        if ! grep -m 1 '^hfst-[^ ]\+ [0-9.]\+' version.out > /dev/null \
        ; then
            rm version.out
            echo "$f has malformed version (when asking from env. var)"
            exit 1
        fi
    fi
done
for f in ../src/hfst-{determinize,invert,minimize,remove-epsilons,reverse,fst2strings,fst2txt,fst2fst,duplicate,name,summarize} ; do
    if [ -x "$f" -a ! -d "$f" ] ; then
        if !  "$f" -i $srcdir/cat.hfst  -o /dev/null > version.out ; then
            rm version.out
            echo "$f has broken version (when asking from env. var)"
            exit 1
        fi
        if ! grep -m 1 '^hfst-[^ ]\+ [0-9.]\+' version.out > /dev/null \
        ; then
            rm version.out
            echo "$f has malformed version (when asking from env. var)"
            exit 1
        fi
    fi
done
rm version.out
export HFST_OPTIONS="$old_opts"
