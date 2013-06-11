#!/bin/sh

SWIGDIR=../debian-test-copy/hfst-3.4.5/swig
PACKAGENAME=hfst_3.4.5_python_amd64

mkdir $PACKAGENAME

# Copy the python files
for file in libhfst.py _libhfst.so;
do
    if ! [ -f $SWIGDIR/$file ]; then
	echo "ERROR: missing file " "'"$SWIGDIR/$file"'"
	exit 1
    fi
    cp $SWIGDIR/$file $PACKAGENAME/;
done

#cp make-install $PACKAGENAME/
cp README $PACKAGENAME/

tar -cvf $PACKAGENAME.tar $PACKAGENAME/
gzip $PACKAGENAME.tar

rm -fR $PACKAGENAME
