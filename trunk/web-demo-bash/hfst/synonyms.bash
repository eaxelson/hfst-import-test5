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
    fiwn-*)
        echo "<div>click terms for wordnet search</div>"
        echo "<dl><dt><a href='http://www.ling.helsinki.fi/cgi-bin/fiwn/search?wn=$LL&w=$WORDFORM&t=over&sm=Search' title='search fiwn for $WORDFORM'>"
        echo $WORDFORM | sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g'
        echo '</a></dt>'
        echo "$WORDFORM" |\
            hfst-lookup hfst/$LL/synonyms.$VARIANT.hfstol 2>&1 |\
            awk -F '\t' "{printf(\"<dd><a href='http://www.ling.helsinki.fi/cgi-bin/fiwn/search?wn=$LL&w=%s&t=over&sm=Search' title='search fiwn for %s'>%s</a></dd>\n\", \$2, \$2, \$2);}" |\
            sort | uniq;
        echo '</dl>';;
    *)
        echo "<pre>";
        echo "$WORDFORM" |\
            hfst-lookup hfst/$LL/synonyms.$VARIANT.hfstol 2>&1 |\
            sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g' |\
            sort | uniq ;
        echo "</pre>";;
esac

