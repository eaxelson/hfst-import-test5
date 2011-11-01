#!/bin/bash
if test $# -lt 3 ; then
    echo "<div>Missing parameters in $0 $@</div>"
    exit 1
fi
WORDFORM=$1
LL=$2
VARIANT=$3
if ! test -r hfst/$LL/dictionary.$VARIANT.hfstol ; then
    echo "<div>Missing hfst/$LL/dictionary.$VARIANT.hfstol</div>"
    exit 1
fi
if ! test -r hfst/$LL/error.$VARIANT.hfstol ; then
    echo "<div>Missing hfst/$LL/dictionary.$VARIANT.hfstol</div>"
    exit 1
fi
echo "<p>Spelling word-form <em>"
echo $WORDFORM | sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g'
echo "</em>:</p>"

echo "<pre>"
echo $WORDFORM |\
    hfst-ospell hfst/$LL/error.$VARIANT.hfstol hfst/$LL/dictionary.$VARIANT.hfstol 2>&1 |\
    sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g'
echo "</pre>"

