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

# Perform the tests
#if (which python2.7 > /dev/null); then
#    ./test.sh --python python2.7
#fi
#if (which python3.2 > /dev/null); then
#    ./test.sh --python python3.2
#fi
./test.sh

echo ""
echo "Exiting directory" `pwd`"..."
echo ""

cd ..
