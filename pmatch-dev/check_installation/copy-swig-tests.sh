#!/bin/sh

#
# Copy tests for HFST swig/Python bindings under ../swig/test to ./swig_tests.
# The copied tests are modifed so that they use the installed versions
# of swig bindings instead of the ones in ../swig.
#

TESTDIR=./swig_tests/

# Copy hfst3/swig/test
cd ../swig/test
files_to_copy=`svn list`
cd ../../check_installation

if [ -d "$TESTDIR" ]; then
    rm -fr $TESTDIR;
fi
mkdir $TESTDIR

for file in $files_to_copy;
do
    cp ../swig/test/$file $TESTDIR
done

cd $TESTDIR

# Not needed
rm Makefile.am

# TODO Modify the tests so that

cd ..
