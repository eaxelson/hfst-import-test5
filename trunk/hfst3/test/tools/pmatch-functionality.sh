#!/bin/sh
TOOLDIR=../../tools/src
if ! $TOOLDIR/hfst-pmatch pmatch_endtag.pmatch < $srcdir/cat.strings > test.lookups ; then
    exit 1
fi

# test equality of output
if ! echo "cat" | $TOOLDIR/hfst-pmatch pmatch_endtag.pmatch > test.pmatch; 
	then
	    exit 1
	fi
	if ! grep -q "<animal>cat</animal>" test.pmatch; then
	    echo "FAIL: cat should be tagged as animal"
	    exit 1
	fi
	
rm TMP
rm test.pmatch
