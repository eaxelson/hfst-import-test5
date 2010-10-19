#!/bin/sh
# does not work until a bug in SFST is fixed 
# (functions reading transducers in binary or text format assume that they are minimal)

for i in foma openfst-tropical sfst # openfst-log
  do
if ! echo "catcatcat(cat)+" | ../../tools/src/hfst-calculate -f $i > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-fst2fst -f $i 4toINFcats.hfst | ../../tools/src/hfst-compare test.hfst > /dev/null 2>&1 ; then
    exit 1
fi
if ! echo "{cat}:{dog}" | ../../tools/src/hfst-calculate -f $i > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-fst2fst -f $i cat2dog.hfst | ../../tools/src/hfst-compare test.hfst > /dev/null 2>&1 ; then
    exit 1
fi ;
  done ;
rm test.hfst
