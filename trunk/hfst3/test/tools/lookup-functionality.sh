#!/bin/sh
if ! ../../tools/src/hfst-lookup cat.hfst < cat.strings > test.lookups ; then
    exit 1
fi

echo "ac" > ac.lookup;
for i in "" .sfst .ofst .foma; do
    if ! ../../tools/src/hfst-lookup abid.hfst$i < ac.lookup > ac.lookups; then
	exit 1
    fi
    if grep "inf" ac.lookups; then
	exit 1
    fi
done

rm test.lookups ac.lookup ac.lookups
