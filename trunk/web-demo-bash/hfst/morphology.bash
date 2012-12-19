#!/bin/bash
if test $# -lt 3 ; then
    echo "<div>Missing parameters in $0 $@</div>"
    exit 1
fi
WORDFORM=${1//+/ }
LL=$2
VARIANT=$3
if ! test -r hfst/$LL/morphology.$VARIANT.hfstol ; then
    echo "<div style='color:red; font-weight: bolder'>Missing or unreadable: <tt>hfst/$LL/morphology.$VARIANT.hfstol</tt></div>"
    exit 1
fi

echo "<p>Analysing word-form(s) <em>"
echo $WORDFORM | sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g'
echo "</em>:</p>"
echo "<pre title='result of hfst-lookup hfst/$LL/morphology.variant.hfstol'"
echo " style='border:black dashed 3px'>"
echo "$WORDFORM" |\
    tr -s '[:space:]' '\n' |\
    hfst-lookup hfst/$LL/morphology.$VARIANT.hfstol 2>&1 |\
    sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g';
echo "</pre>"
