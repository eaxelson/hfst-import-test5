#!/bin/sh
if ! test -x ../../tools/src/hfst-calculate; then
    exit 0;
fi

for i in "" .sfst .ofst .foma; do
    FFLAG=
    case $i in
        .sfst)
            FFLAG="-f sfst"
	    if ! (../../tools/src/hfst-format --list-formats | grep "sfst" > /dev/null) ; then
		continue;
	    fi;;
        .ofst)
            FFLAG="-f openfst-tropical"
	    if ! (../../tools/src/hfst-format --list-formats | grep "openfst-tropical" > /dev/null) ; then
		continue;
	    fi;;
        .foma)
            FFLAG="-f foma"
	    if ! (../../tools/src/hfst-format --list-formats | grep "foma" > /dev/null) ; then
		continue;
	    fi;;
        *)
            FFLAG=;;
    esac
    if test -f 4toINFcats$i ; then
        if ! echo "catcatcat(cat)+" | ../../tools/src/hfst-calculate $FFLAG > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s 4toINFcats$i test  ; then
            exit 1
        fi
        rm test
    fi
    if test -f cat2dog$i ; then
        if ! echo "{cat}:{dog}" | ../../tools/src/hfst-calculate $FFLAG > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s cat2dog$i test  ; then
            exit 1
        fi
        rm test
    fi
done
