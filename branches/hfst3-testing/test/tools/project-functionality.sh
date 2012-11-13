#!/bin/sh
for i in  .hfst .sfst .ofst .foma; do

    if test -f cat2dog$i -a -f cat$i -a -f dog$i ; then
        if ! ../../tools/src/hfst-project -p input cat2dog$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test cat$i  ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-project -p output cat2dog$i > test ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare -s test dog$i  ; then
            exit 1
        fi
        rm test;
	for j in input output; do
	    if ! ../../tools/src/hfst-project -p $j cat2dog$i > test ; then
		exit 1
	    fi
	    if ! ../../tools/src/hfst-concatenate test unk2unk$i > concatenation; then
		exit 1
	    fi
	done
    fi
    # test that the input/output side alphabet is present, too
    if ! ../../tools/src/hfst-fst2txt concatenation | egrep -q "d|o|g" ; then
	echo "FAIL: The alphabet of the output side is not present" $i;
	exit 1
    fi
    if ! ../../tools/src/hfst-project -p $j concatenation > concatenation.projection; then
	exit 1;
    fi
    # test that there are no unknowns in projection
    if ../../tools/src/hfst-fst2txt concatenation.projection | grep -q "@_UNKNOWN_SYMBOL_@" ; then
	echo "FAIL: Unknowns in " $j " projection (should be identities)" $i;
	exit 1
    fi
done
rm concatenation concatenation.projection

