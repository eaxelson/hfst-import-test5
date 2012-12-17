#!/bin/sh

extension=.sh
languages="english finnish french german italian swedish turkish"
directions="analyze generate"
format=xerox
morph_folder=morphology_tests

function fail {
    rm -f input tmp
    echo "FAIL"
    exit 1
}

for lang in $languages;
do
    for dir in $directions;
    do
	prog=$lang-$dir$extension
	echo -n "Testing "$prog"... "

	# test that the program exists
	if (! which $prog 2>1 > /dev/null); then
	    fail
	fi    

	# test that the program handles a non-word 
	rm -f input
	echo "foo" > input
	if (! $prog $format input 2>1 > /dev/null); then
	    fail
	fi

	if (! cat input | $prog $format 2>1 > /dev/null); then
	    fail
	fi

	# test that the program handles a real word
	if (! $prog $format $morph_folder/$lang-$dir.input > tmp); then
	    fail
	fi
	if (! diff tmp $morph_folder/$lang-$dir.output); then
	    fail
	fi

	if (! cat $morph_folder/$lang-$dir.input | $prog $format > tmp); then
	    fail
	fi
	if (! diff tmp $morph_folder/$lang-$dir.output); then
	    fail
	fi

	echo "PASS"
    done
done

rm -f input tmp

echo "-----------------"
echo "All tests passed."
echo "-----------------"
