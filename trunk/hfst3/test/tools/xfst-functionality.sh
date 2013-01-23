#!/bin/sh
SCRIPTDIR=../../scripts/
TOOLDIR=../../tools/src/

if ! test -x $SCRIPTDIR/hfst-xfst-loc; then
    exit 0;
fi

for format in sfst openfst-tropical foma; do
    
    if ($TOOLDIR/hfst-format --list-formats | grep $format > /dev/null) ; then

	if ! ($SCRIPTDIR/hfst-xfst-loc -F $format -f script.xfst) 2>1 > /dev/null; then
	    rm -f script.hfst
	    exit 1;
	fi

	if ! echo "a:b" | $TOOLDIR/hfst-strings2fst -f $format | $TOOLDIR/hfst-compare -s script.hfst; then
	    rm -f script.hfst
	    exit 1;
	fi

    fi

done

rm -f script.hfst
