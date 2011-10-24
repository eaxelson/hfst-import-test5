#!/bin/sh
if echo "a:b" | ../../tools/src/hfst-calculate -f sfst > test.hfst ; then
    if ! ../../tools/src/hfst-format test.hfst > TMP1 ; then
	exit 1
    fi
    echo "Transducers in test.hfst are of type SFST (1.4 compatible)" > TMP2
    if ! diff TMP1 TMP2 ; then
	exit 1
    fi
fi

if echo "a:b" | ../../tools/src/hfst-calculate -f openfst-tropical \
    > test.hfst ; then
    if ! ../../tools/src/hfst-format test.hfst > TMP1 ; then
	exit 1
    fi
    echo "Transducers in test.hfst are of type OpenFST, std arc,"\
" tropical semiring" > TMP2
    if ! diff TMP1 TMP2 ; then
	exit 1
    fi
fi

if echo "a:b" | ../../tools/src/hfst-calculate -f foma > test.hfst ; then
    if ! ../../tools/src/hfst-format test.hfst > TMP1 ; then
	exit 1
    fi
    echo "Transducers in test.hfst are of type foma" > TMP2
    if ! diff TMP1 TMP2 ; then
	exit 1
    fi
fi

rm test.hfst
rm TMP1
rm TMP2
