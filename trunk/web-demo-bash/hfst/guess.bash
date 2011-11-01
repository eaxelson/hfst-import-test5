#!/bin/bash 
if test $# -lt 3 ; then
    echo "<div>Missing parameters in $0 $@</div>"
    exit 1
fi
WORDFORM=$1
LL=$2
VARIANT=$3
if ! test -r hfst/$LL/guess.$VARIANT.hfst ; then
    echo "<div>Missing hfst/$LL/guess.$VARIANT.hfst</div>"
    exit 1
fi
echo "<p>Guessing word-form <em>"
echo $WORDFORM | sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g'
echo "</em>:</p>"
if test -r hfst/$LL/dictionary.hfstol ; then
    if ! echo $WORDFORM | hfst-optimized-lookup hfst/$LL/dictionary.hfstol | grep '+?' > /dev/null ; then
        echo "<p>The word already exists in dictionary.</p>"
    fi
fi
echo "<pre>"
for guess in $(echo $WORDFORM | hfst-strings2fst2 -R hfst/$LL/symbols -w | hfst-compose2 - hfst/$LL/guess.$VARIANT.hfst | hfst-strings2fst2 -w -n 20 | cut -f 1) ; do
    if -x generate.bash ; then
        ./generate.bash $guess $LL $VARIANT
    else
        echo $guess
    fi
done
echo "</pre>"

