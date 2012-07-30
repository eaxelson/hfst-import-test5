#!/bin/sh
if ! test -x ../src/hfst-calculate; then
    exit 0;
fi

for i in "" .sfst .ofst .foma; do
    FFLAG=
    case $i in
        .sfst)
            FFLAG="-f sfst"
	    if ! (../src/hfst-format --list-formats | grep "sfst") ; then
		continue;
	    fi;;
        .ofst)
            FFLAG="-f openfst-tropical"
	    if ! (../src/hfst-format --list-formats | grep "openfst-tropical") ; then
		continue;
	    fi;;
        .foma)
            FFLAG="-f foma"
	    if ! (../src/hfst-format --list-formats | grep "foma") ; then
		continue;
	    fi;;
        *)
            FFLAG=;;
    esac
    if test -f 4toINFcats.hfst$i ; then
        if ! echo "catcatcat(cat)+" | ../src/hfst-calculate $FFLAG > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare 4toINFcats.hfst$i test.hfst  ; then
            exit 1
        fi
        rm test.hfst
    fi
    if test -f cat2dog.hfst$i ; then
        if ! echo "{cat}:{dog}" | ../src/hfst-calculate $FFLAG > test.hfst ; then
            exit 1
        fi
        if ! ../src/hfst-compare cat2dog.hfst$i test.hfst  ; then
            exit 1
        fi
        rm test.hfst
    fi
done
