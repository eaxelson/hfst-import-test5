#!/bin/sh
for i in openfst-tropical # foma sfst # openfst-log
  do
if ! echo "cat|dog" | ../../tools/src/hfst-calculate -f $i > lexicon.hfst ; then
    exit 1
fi
if ! echo "([catdog]|d:D|c:C)*" | ../../tools/src/hfst-calculate -f $i > rule1.hfst ; then
    exit 1
fi
if ! echo "([catdog]|d:D|c:C)*" | ../../tools/src/hfst-calculate -f $i > rule2.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compose-intersect -l lexicon.hfst rule1.hfst rule2.hfst > TMP ; then
    exit 1
fi
if ! echo "cat|dog|{cat}:{Cat}|{dog}:{Dog}" | ../../tools/src/hfst-calculate > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-compare test.hfst TMP > /dev/null 2>&1 ; then
    exit 1
fi ;
  done ;
rm test.hfst
rm TMP
rm lexicon.hfst
rm rule1.hfst
rm rule2.hfst
