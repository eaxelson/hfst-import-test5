#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f heavycat$i ; then
        if ! ../../tools/src/hfst-reweight -a 1 heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -b 3.141 heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -a 10 -b 0.001 heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -F cos heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -a 11 -b 0.5 -F sin heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -l 1000 -a 1 heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -u 100000 -a 1 heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -l 1000 -u 100000 -a 1 heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -S c -a 1 heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -I c -a 1 heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -O c -a 1 heavycat$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-reweight -I c -O c -a 1 heavycat$i > test ; then
            exit 1
        fi
    fi
done

