#!/bin/bash
if test $# -lt 3 ; then
    echo "<div>Missing parameters in $0 $@</div>"
    exit 1
fi
WORDFORM=$1
LL=$2
VARIANT=$3
if ! test -r hfst/$LL/translation.$VARIANT.hfstol ; then
    echo "<div>Missing hfst/$LL/translation.$VARIANT.hfstol</div>"
    exit 1
fi

echo "<p>Finding translations for <em>"
echo $WORDFORM | sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g'
echo "</em>:</p>"
        
echo "<pre title='Output of hfst-lookup hfst/fi/translation.$VARIANT.hfstol'"
echo " style='border: 3px dashed black;'>"
echo "$WORDFORM" |\
    hfst-lookup hfst/$LL/translation.$VARIANT.hfstol 2>&1 |\
    sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g' |\
    sort | uniq ;
echo "</pre>"

