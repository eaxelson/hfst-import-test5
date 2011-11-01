#!/bin/bash
if test $# -lt 3 ; then
    echo "<div>Missing parameters in $0 $@</div>"
    exit 1
fi
WORDFORM=$1
LL=$2
VARIANT=$3

if ! test -r hfst/$LL/generation.$VARIANT.hfst ; then
    echo "<div>Missing hfst/$LL/generation.$VARIANT.hfst</div>"
    exit 1
fi
echo "<p>Creating word-forms for lemma <em>"
echo $WORDFORM | sed -e 's/&/\&amp;/g' -e 's/</&lt;/g'
echo "</em>:</p>"

echo "<pre>"
echo "$WORDFORM ?*" | hfst-regexp2fst -R hfst/$LL/symbols -w | hfst-compose - hfst/$LL/guess.$VARIANT.hfst | hfst-strings2fst -w -n 20
echo "</pre>"

