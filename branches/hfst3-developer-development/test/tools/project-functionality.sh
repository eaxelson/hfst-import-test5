#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a -f cat.hfst$i -a -f dog.hfst$i ; then
        if ! ../../tools/src/hfst-project -p input cat2dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat.hfst$i  ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-project -p output cat2dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst dog.hfst$i  ; then
            exit 1
        fi
        rm test.hfst;
	for j in input output; do
	    if ! ../../tools/src/hfst-project -p $j cat2dog.hfst$i > test.hfst ; then
		exit 1
	    fi
	    if ! ../../tools/src/hfst-concatenate test.hfst unk2unk.hfst$i > concatenation.hfst; then
		exit 1
	    fi
	done
    fi
    # test that the input/output side alphabet is present, too
    if ! ../../tools/src/hfst-fst2txt concatenation.hfst | egrep -q "d|o|g" ; then
	echo "FAIL: The alphabet of the output side is not present" $i;
	exit 1
    fi
    if ! ../../tools/src/hfst-project -p $j concatenation.hfst > concatenation.projection.hfst; then
	exit 1;
    fi
    # test that there are no unknowns in projection
    if ../../tools/src/hfst-fst2txt concatenation.projection.hfst | grep -q "@_UNKNOWN_SYMBOL_@" ; then
	echo "FAIL: Unknowns in " $j " projection (should be identities)" $i;
	exit 1
    fi
done
rm concatenation.hfst concatenation.projection.hfst

