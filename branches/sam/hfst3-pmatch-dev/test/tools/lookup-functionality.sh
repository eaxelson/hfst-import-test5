#!/bin/sh
TOOLDIR=../../tools/src
if ! $TOOLDIR/hfst-lookup -s cat.hfst < $srcdir/cat.strings > test.lookups ; then
    exit 1
fi

$TOOLDIR/hfst-format -l > TMP;

# test what strings the transducer [a:b (ID:ID)*] recognizes
for i in "" .sfst .ofst .foma; do

    if test -f abid$i ; then

	if ! echo "aa" | $TOOLDIR/hfst-lookup -s abid$i \
	    > test.lookups; 
	then
	    exit 1
	fi
	if ! grep -q "inf" test.lookups; then
	    echo "FAIL: string 'aa' should not be recognized"
	    exit 1
	fi
	
	if ! echo "ab" | $TOOLDIR/hfst-lookup -s abid$i \
	    > test.lookups; 
	then
	    exit 1
	fi
	if ! grep -q "inf" test.lookups; then
	    echo "FAIL: string 'ab' should not be recognized"
	    exit 1
	fi
	
	if ! echo "ac" | $TOOLDIR/hfst-lookup -s abid$i \
	    > test.lookups; 
	then
	    exit 1
	fi
	if grep -q "inf" test.lookups; then
	    echo "FAIL: string 'ac' should be recognized"
	    exit 1
	fi

    fi

done

rm TMP
rm test.lookups