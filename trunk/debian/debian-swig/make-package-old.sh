#!/bin/sh

SWIGDIR=../debian-test-copy/hfst-3.5.0/swig
PACKAGENAME=hfst_3.5.0_python_amd64
FILES="_libhfst.so _libhfst.cpython-32mu.so python2-libhfst.py python3-libhfst.py"

mkdir $PACKAGENAME
mkdir $PACKAGENAME/python2
mkdir $PACKAGENAME/python3

# Test that all files exist
for file in $FILES
do
    if ! [ -f $SWIGDIR/$file ]; then
	echo "ERROR: missing file " "'"$SWIGDIR/$file"'"
	exit 1
    fi
done

# Copy the files
cp $SWIGDIR/_libhfst.so $PACKAGENAME/python2/
cp $SWIGDIR/_libhfst.cpython-32mu.so $PACKAGENAME/python3/
cp $SWIGDIR/python2-libhfst.py $PACKAGENAME/python2/libhfst.py
cp $SWIGDIR/python3-libhfst.py $PACKAGENAME/python3/libhfst.py

cp README $PACKAGENAME/

tar -cvf $PACKAGENAME.tar $PACKAGENAME/
gzip $PACKAGENAME.tar

rm -fR $PACKAGENAME
