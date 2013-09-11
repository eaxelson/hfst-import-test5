#!/bin/sh

#
# Perform tests in ./swig_tests. Before executing this script,
# run copy-swig-tests.sh to create ./swig_tests.
#

TESTDIR=swig_tests

if [ ! -d "$TESTDIR" ]; then
    echo "ERROR: directory" $TESTDIR "does not exist, try running ./copy-swig-tests.sh first."
    exit 1;
fi

echo "---------------------------------------- "
echo "Testing SWIG/Python bindings for HFST... "
echo "---------------------------------------- "

echo ""
echo "Moving to directory" `pwd`"/"$TESTDIR"..."
echo ""
cd $TESTDIR

./test.sh --python python3

echo ""
echo "Exiting directory" `pwd`"..."
echo ""

cd ..
