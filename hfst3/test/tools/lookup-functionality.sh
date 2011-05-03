#!/bin/sh
if ! ../../tools/src/hfst-lookup cat.hfst < $srcdir/cat.strings > test.lookups ; then
    exit 1
fi

# test what strings the transducer [a:b (ID:ID)*] recognizes
for i in "" .sfst .ofst .foma; do

    if ! echo "aa" | ../../tools/src/hfst-lookup abid.hfst$i > test.lookups; 
    then
	exit 1
    fi
    if ! grep -q "inf" test.lookups; then
	echo "FAIL: string 'aa' should not be recognized"
	exit 1
    fi

    if ! echo "ab" | ../../tools/src/hfst-lookup abid.hfst$i > test.lookups; 
    then
	exit 1
    fi
    if ! grep -q "inf" test.lookups; then
	echo "FAIL: string 'ab' should not be recognized"
	exit 1
    fi

    if ! echo "ac" | ../../tools/src/hfst-lookup abid.hfst$i > test.lookups; 
    then
	exit 1
    fi
    if grep -q "inf" test.lookups; then
	echo "FAIL: string 'ac' should be recognized"
	exit 1
    fi

done

rm test.lookups
