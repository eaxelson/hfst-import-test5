#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a cat.hfst$i -a dog.hfst$i ; then
        if ! ../../tools/src/hfst-project -p input cat2dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat.hfst$i > /dev/null 2>&1 ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-project -p output cat2dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst dog.hfst$i > /dev/null 2>&1 ; then
            exit 1
        fi
        rm test.hfst;
    fi
    if ! ../../tools/src/hfst-project -p input cat2dog.hfst$i > test.hfst ; then
	exit 1
    fi
    if ! ../../tools/src/hfst-concatenate test.hfst unk2unk.hfst$i > concatenation.hfst; then
	exit 1
    fi
    # test that the input side alphabet is present, too
    if ! ../../tools/src/hfst-fst2txt concatenation.hfst | egrep -q "d|o|g" ; then
	echo "FAIL: The alphabet of the output side is not present" $i;
	exit 1
    fi
    if ! ../../tools/src/hfst-project -p input concatenation.hfst > concatenation.input.hfst; then
	exit 1;
    fi
    # test that there are no unknowns in projection
    if ../../tools/src/hfst-fst2txt concatenation.input.hfst$i | grep -q "@_UNKNOWN_SYMBOL_@" ; then
	echo "FAIL: Unknowns in input projection (should be identities)" $i;
	exit 1
    fi
done
