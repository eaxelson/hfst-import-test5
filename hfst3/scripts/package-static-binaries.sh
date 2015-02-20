#!/bin/sh

if [ "$1" = "-h" ]; then
    echo "Usage: $0 DIRNAME";
    exit 0;
fi

if [ "$1" = "" ]; then
    echo "Usage: $0 DIRNAME";
    exit 1;
fi

if [ -d "$1" ]; then
    echo "directory $1 exists"
    exit 1;
else
    mkdir $1;
fi

# package all eight statically compiled tools
cp tools/src/parsers/.libs/hfst-xfst $1/
cp tools/src/.libs/hfst-lexc $1/
cp tools/src/.libs/hfst-lookup $1/
cp tools/src/.libs/hfst-optimized-lookup $1/
cp tools/src/.libs/hfst-pmatch $1/
cp tools/src/.libs/hfst-pmatch2fst $1/
cp tools/src/hfst-proc/.libs/hfst-apertium-proc $1/hfst-proc
cp tools/src/hfst-twolc/src/.libs/htwolcpre1 $1/
cp tools/src/hfst-twolc/src/.libs/htwolcpre2 $1/
cp tools/src/hfst-twolc/src/.libs/htwolcpre3 $1/
cp scripts/hfst-twolc-bin $1/hfst-twolc
