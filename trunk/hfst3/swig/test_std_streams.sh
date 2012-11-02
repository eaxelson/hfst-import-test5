#!/bin/sh

TOOLDIR=../tools/src/
STRINGS2FST="$TOOLDIR"hfst-strings2fst
COMPARE="$TOOLDIR"hfst-compare" -s"
PYTHON=$1

STRING1="abcdefghijklmnopqrstuvwxyz"
STRING2="ABCDEFGHIJKLMNOPQRSTUVWXYZ"

echo $STRING1 | $STRINGS2FST -f sfst > tr1.sfst
echo $STR1NG2 | $STRINGS2FST -f sfst > tr2.sfst
cat tr1.sfst tr2.sfst > tr.sfst
echo $STRING1 | $STRINGS2FST -f openfst-tropical > tr1.ofst
echo $STRING2 | $STRINGS2FST -f openfst-tropical > tr2.ofst
cat tr1.ofst tr2.ofst > tr.ofst
echo $STRING1 | $STRINGS2FST -f foma > tr1.foma
echo $STRING2 | $STRINGS2FST -f foma > tr2.foma
cat tr1.foma tr2.foma > tr.foma

if ! (cat tr.sfst | $PYTHON ./test_std_streams.py sfst 2 > tmp.sfst); then
    echo "ERROR: in sfst"
    exit 1
fi
if ! ($COMPARE tr.sfst tmp.sfst); then
    echo "ERROR in sfst"
    exit 1
fi
if ! (cat tr.ofst | $PYTHON ./test_std_streams.py openfst 2 > tmp.ofst); then
    echo "ERROR: in openfst"
    exit 1
fi
if ! ($COMPARE tr.ofst tmp.ofst); then
    echo "ERROR: in openfst"
    exit 1
fi
if ! (cat tr.foma | $PYTHON ./test_std_streams.py foma 2 > tmp.foma); then
    echo "ERROR: in foma"
    exit 1
fi  
if ! ($COMPARE tr.foma tmp.foma); then
    echo "ERROR: in foma"
    exit 1
fi  