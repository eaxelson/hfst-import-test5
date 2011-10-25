#!/bin/sh
if ! test -x ../../tools/src/hfst-calculate; then
    exit 0;
fi

for i in "" .sfst .ofst .foma; do
    FFLAG=
    case $i in
        .sfst)
            FFLAG="-f sfst"
	    if ! (../../tools/src/hfst-format --list-formats | grep "sfst") ; then
		continue;
	    fi;;
        .ofst)
            FFLAG="-f openfst-tropical"
	    if ! (../../tools/src/hfst-format --list-formats | grep "openfst-tropical") ; then
		continue;
	    fi;;
        .foma)
            FFLAG="-f foma"
	    if ! (../../tools/src/hfst-format --list-formats | grep "foma") ; then
		continue;
	    fi;;
        *)
            FFLAG=;;
    esac
    if test -f 4toINFcats.hfst$i ; then
        if ! echo "catcatcat(cat)+" | ../../tools/src/hfst-calculate $FFLAG > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s 4toINFcats.hfst$i test.hfst  ; then
            exit 1
        fi
        rm test.hfst
    fi
    if test -f cat2dog.hfst$i ; then
        if ! echo "{cat}:{dog}" | ../../tools/src/hfst-calculate $FFLAG > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s cat2dog.hfst$i test.hfst  ; then
            exit 1
        fi
        rm test.hfst
    fi
done
