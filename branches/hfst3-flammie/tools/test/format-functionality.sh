#!/bin/sh

echo '0 1 a b
1' > TMP;

if ../src/hfst-format --test-format sfst; then
    if echo TMP | ../src/hfst-txt2fst -f sfst > test.hfst ; then
	if ! ../src/hfst-format test.hfst > TMP1 ; then
	    exit 1
	fi
	echo "Transducers in test.hfst are of type SFST (1.4 compatible)" > TMP2
	if ! diff TMP1 TMP2 ; then
	    exit 1
	fi
    fi
fi

if ../src/hfst-format --test-format openfst-tropical; then
    if echo TMP | ../src/hfst-txt2fst -f openfst-tropical \
	> test.hfst ; then
	if ! ../src/hfst-format test.hfst > TMP1 ; then
	    exit 1
	fi
	echo "Transducers in test.hfst are of type OpenFST, std arc,"\
             "tropical semiring" > TMP2
	if ! diff TMP1 TMP2 ; then
	    exit 1
	fi
    fi
fi

if ../src/hfst-format --test-format foma; then
    if echo TMP | ../src/hfst-txt2fst -f foma > test.hfst ; then
	if ! ../src/hfst-format test.hfst > TMP1 ; then
	    exit 1
	fi
	echo "Transducers in test.hfst are of type foma" > TMP2
	if ! diff TMP1 TMP2 ; then
	    exit 1
	fi
    fi
fi

rm -f test.hfst
rm -f TMP
rm -f TMP1
rm -f TMP2