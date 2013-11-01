#!/bin/sh

#
# Check all installed HFST morphologies. 
#

prefix=
if [ "$1" = "--prefix" ]; then
    prefix=$2
fi


extension=.sh
languages="english finnish french german italian omorfi swedish turkish"
directions="analyze generate"
format=xerox
morph_folder=morphology_tests

function exit_prog {
    rm -f input tmp
    exit 1
}

echo "---------------------------- "
echo "Testing HFST morphologies... "
echo "---------------------------- "

for lang in $languages;
do
    for dir in $directions;
    do
	prog=$lang-$dir$extension

	# test that the program exists
        if [ "$prefix" = "" ]; then
	    if (! which $prog 2>1 > /dev/null); then
                printf "%-32s%s\n" $prog "FAIL: program not found"
                exit_prog
	    fi    
        else
            if ! [ -x $prefix$prog ]; then
                printf "%-32s%s\n" $prog "FAIL: program not found or executable"
                exit_prog
            fi
        fi  

	# test that the program handles a non-word 
        rm -f input 
        echo "foo" > input 
        if (! $prefix$prog $format input 2>1 > /dev/null); then
	    printf "%-32s%s\n" $prog "FAIL: program cannot handle input 'foo' (given as first argument)"
            exit_prog 
        fi

	if (! cat input | $prefix$prog $format 2>1 > /dev/null); then
	    printf "%-32s%s\n" $prog "FAIL: program cannot handle input 'foo' (given via standard input)"
            exit_prog
	fi

	# test that the program handles a real word
	if (! $prefix$prog $format $morph_folder/$lang-$dir.input > tmp); then
	    printf "%-32s%s\n" $prog "FAIL: program cannot handle valid input (given as first argument)"
            exit_prog
	fi
	if (! diff tmp $morph_folder/$lang-$dir.output); then
	    printf "%-32s%s\n" $prog "FAIL: wrong result for input (given as first argument)"
            exit_prog
	fi

	if (! cat $morph_folder/$lang-$dir.input | $prefix$prog $format > tmp); then
	    printf "%-32s%s\n" $prog "FAIL: program cannot handle valid input (given via standard input)"
            exit_prog
	fi
	if (! diff tmp $morph_folder/$lang-$dir.output); then
	    printf "%-32s%s\n" $prog "FAIL: wrong result for input (given via standard input)"
            exit_prog
	fi

	printf "%-32s%s\n" $prog "PASS"
    done
done

rm -f input tmp

echo "-----------------"
echo "All tests passed."
echo "-----------------"
