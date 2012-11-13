#!/bin/sh

echo '0 1 a b
1' > TMP;

if ../../tools/src/hfst-format --test-format sfst; then
    if echo TMP | ../../tools/src/hfst-txt2fst -f sfst > test ; then
	if ! ../../tools/src/hfst-format test > TMP1 ; then
	    exit 1
	fi
	echo "Transducers in test are of type SFST (1.4 compatible)" > TMP2
	if ! diff TMP1 TMP2 ; then
	    exit 1
	fi
    fi
fi

if ../../tools/src/hfst-format --test-format openfst-tropical; then
    if echo TMP | ../../tools/src/hfst-txt2fst -f openfst-tropical \
	> test ; then
	if ! ../../tools/src/hfst-format test > TMP1 ; then
	    exit 1
	fi
	echo "Transducers in test are of type OpenFST, std arc,"\
             "tropical semiring" > TMP2
	if ! diff TMP1 TMP2 ; then
	    exit 1
	fi
    fi
fi

if ../../tools/src/hfst-format --test-format foma; then
    if echo TMP | ../../tools/src/hfst-txt2fst -f foma > test ; then
	if ! ../../tools/src/hfst-format test > TMP1 ; then
	    exit 1
	fi
	echo "Transducers in test are of type foma" > TMP2
	if ! diff TMP1 TMP2 ; then
	    exit 1
	fi
    fi
fi

rm -f test
rm -f TMP
rm -f TMP1
rm -f TMP2
