#!/bin/bash
if test $# -lt 2 ; then
    echo "<div>Usage: $0 WORDFORM VARIANT</div>"
    exit 1
fi

WORDFORM=$1
LL='fi'
VARIANT=$2

if ! test -r hfst/$LL/morphology.$VARIANT.hfstol ; then
    echo "<div>Missing hfst/$LL/morphology.$VARIANT.hfstol</div>"
    exit 1
fi

echo "<p>Analysing word-form <em>"
echo $WORDFORM | sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g'
echo "</em>:</p>"

case $VARIANT in
    cg)
        echo "<div>click terms for wordnet search on form / stem</div>"
        echo "<dl><dt><a href='http://www.ling.helsinki.fi/cgi-bin/fiwn/search?wn=fi&w=$WORDFORM&t=over&sm=Search' title='search fiwn for Finnish $WORDFORM'>"
        echo $WORDFORM | sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g'
        echo '</a></dt>'
        echo "$WORDFORM" |\
            hfst-lookup hfst/$LL/morphology.$VARIANT.hfstol 2>&1 |\
            awk '{printf("<dd><a href=\"http://www.ling.helsinki.fi/cgi-bin/fiwn/search?wn=fi&w=%s&t=over&sm=Search\" title=\"search fiwn for Finnish %s\">%s</a></dd>\n", gensub(/+.*/, "", "g", $2), gensub(/+.*/, "", "g", $2), $2);}';
        echo '</dl>';;
    *)
        echo "<pre>";
        echo "$WORDFORM" |\
            hfst-lookup hfst/$LL/morphology.$VARIANT.hfstol 2>&1 |\
            sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g';
        echo "</pre>";;
esac
