#!/bin/sh

SWIGDIR=../../hfst3/swig
PACKAGENAME=hfst-22-swig

# Copy the python files
for file in libhfst.py _libhfst.so;
do
    if ! [ -f $SWIGDIR/$file ]; then
	echo "ERROR: missing file " "'"$SWIGDIR/$file"'"
	exit 1
    fi
    cp $SWIGDIR/$file .;
done

tar -cvf $PACKAGENAME.tar libfst.py _libhfst.so make-install README
gz $PACKAGENAME.tar
