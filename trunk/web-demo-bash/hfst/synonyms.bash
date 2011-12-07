#!/bin/bash
if test $# -lt 3 ; then
    echo "<div>Missing parameters in $0 $@</div>"
    exit 1
fi
WORDFORM=$1
LL=$2
VARIANT=$3
if ! test -r hfst/$LL/synonyms.$VARIANT.hfstol ; then
    echo "<div>Missing hfst/$LL/synonyms.$VARIANT.hfstol</div>"
    exit 1
fi

echo "<p>Finding synonyms for <em>"
echo $WORDFORM | sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g'
echo "</em>:</p>"
case $VARIANT in
    *)
        echo "<pre>";
        echo "$WORDFORM" |\
            hfst-lookup hfst/$LL/synonyms.$VARIANT.hfstol 2>&1 |\
            sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g' |\
            sort | uniq ;
        echo "</pre>";;
esac

