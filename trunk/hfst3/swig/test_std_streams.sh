#!/bin/sh

TOOLDIR=../tools/src/
CALCULATE="$TOOLDIR"hfst-calculate
COMPARE="$TOOLDIR"hfst-compare" -s"

echo "a:b" | $CALCULATE -f sfst > a2b.sfst
echo "c:d" | $CALCULATE -f sfst > c2d.sfst
cat a2b.sfst c2d.sfst > tr.sfst
echo "a:b" | $CALCULATE -f openfst-tropical > a2b.ofst
echo "c:d" | $CALCULATE -f openfst-tropical > c2d.ofst
cat a2b.ofst c2d.ofst > tr.ofst
echo "a:b" | $CALCULATE -f foma > a2b.foma
echo "c:d" | $CALCULATE -f foma > c2d.foma
cat a2b.foma c2d.foma > tr.foma

if ! (cat tr.sfst | python ./test_std_streams.py sfst 2 > tmp.sfst); then
    echo "ERROR: in sfst"
    exit 1
fi
if ! ($COMPARE tr.sfst tmp.sfst); then
    echo "ERROR in sfst"
    exit 1
fi
if ! (cat tr.ofst | python ./test_std_streams.py openfst 2 > tmp.ofst); then
    echo "ERROR: in openfst"
    exit 1
fi
if ! ($COMPARE tr.ofst tmp.ofst); then
    echo "ERROR: in openfst"
    exit 1
fi
if ! (cat tr.foma | python ./test_std_streams.py foma 2 > tmp.foma); then
    echo "ERROR: in foma"
    exit 1
fi  
if ! ($COMPARE tr.foma tmp.foma); then
    echo "ERROR: in foma"
    exit 1
fi  