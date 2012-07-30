#!/bin/sh

# default test, basics
if ! ../src/hfst-lookup -s cat.hfst < $srcdir/cat.strings > test.lookups ; then
    exit 1
fi

# test what strings the transducer [a:b (ID:ID)*] recognizes
for i in "" .sfst .ofst .foma; do
    if test -f abid.hfst$i ; then
        if ! echo "aa" | ../src/hfst-lookup abid.hfst$i \
            > test.lookups; 
        then
            exit 1
        fi
        if ! grep -q "inf" test.lookups; then
            echo "FAIL: string 'aa' should not be recognized"
            exit 1
        fi
        if ! echo "ab" | ../src/hfst-lookup abid.hfst$i \
            > test.lookups; 
        then
            exit 1
        fi
        if ! grep -q "inf" test.lookups; then
            echo "FAIL: string 'ab' should not be recognized"
            exit 1
        fi
        if ! echo "ac" | ../src/hfst-lookup abid.hfst$i \
            > test.lookups; 
        then
            exit 1
        fi
        if grep -q "inf" test.lookups; then
            echo "FAIL: string 'ac' should be recognized"
            exit 1
        fi
        if ! cat $srcdir/long.strings | ../src/hfst-lookup abid.hfst$i > test.lookups ; then
            echo "65536 character lookup fail"
            exit 1
        fi
    fi
done

rm test.lookups
