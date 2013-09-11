#!/bin/sh

#
# Perform tests in ./tool_tests. Before executing this script,
# run copy-tool-tests.sh to create ./tool_tests.
#

PREFIX=

if [ "$1" = "-h" -o "$1" = "--help" ]; then
    echo ""
    echo "  Usage:  "$0" [--prefix PREFIX]"
    echo ""
    exit 0;
elif [ "$1" = "--prefix" ]; then
    PREFIX=$2"/";
fi

TESTDIR=tool_tests

if [ ! -d "$TESTDIR" ]; then
    echo "ERROR: directory" $TESTDIR "does not exist, try running ./copy-tool-tests.sh first."
    exit 1;
fi

echo "---------------------------------- "
echo "Testing HFST command line tools... "
echo "---------------------------------- "

echo ""
echo "Moving to directory" `pwd`"/"$TESTDIR"..."
echo ""
cd $TESTDIR

# Make the transducers needed by the tests.
# Overgenerating some files here..
for att_file in *.txt;
do
    file=`echo $att_file | sed 's/\.txt//'`
    hfst-txt2fst -e '@0@' $file.txt > $file.hfst
    if (hfst-format --list-formats | grep 'sfst' > /dev/null); then
	hfst-txt2fst -f sfst -e '@0@' $file.txt > $file.sfst
    else
	echo "warning: sfst back-end not available, assumed skipped off and continuing"
    fi
    hfst-txt2fst -f openfst-tropical -e '@0@' $file.txt > $file.ofst
    if (hfst-format --list-formats | grep 'foma' > /dev/null); then
	hfst-txt2fst -f foma -e '@0@' $file.txt > $file.foma
    else
	echo "warning: foma back-end not available, assumed skipped off and continuing"
    fi
    hfst-txt2fst -e '@0@' -i $file.txt | hfst-fst2fst -w -o $file.hfstol
    hfst-txt2fst -e '@0@' -i $file.txt | hfst-invert | hfst-fst2fst -w -o $file.genhfstol
done

# Perform the tests
for tooltest in *.sh;
do
    if [ "$tooltest" != "copy-files.sh" -a \
	"$tooltest" != "lexc2fst-stress.sh" -a \
	"$tooltest" != "valgrind.sh" -a \
	"$tooltest" != "lookup-stress.sh" ]; then
	echo -n "Testing: "$tooltest"...   "
	if (./$tooltest $PREFIX) ; then
	    echo "PASS"
	elif [ "$?" -eq "77" ] ; then
            echo "SKIP"
        else
	    echo "FAIL"
	fi
    fi
done

echo ""
echo "Exiting directory" `pwd`"..."
echo ""

cd ..
