#!/bin/sh
if ! echo "a:b" | ../../tools/src/hfst-calculate -f sfst > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-format test.hfst > TMP1 ; then
    exit 1
fi
echo "SFST_TYPE" > TMP2
if ! diff TMP1 TMP2 ; then
    exit 1
fi
if ! echo "a:b" | ../../tools/src/hfst-calculate -f openfst-tropical > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-format test.hfst > TMP1 ; then
    exit 1
fi
echo "OPENFST_TROPICAL_TYPE" > TMP2
if ! diff TMP1 TMP2 ; then
    exit 1
fi
if ! echo "a:b" | ../../tools/src/hfst-calculate -f foma > test.hfst ; then
    exit 1
fi
if ! ../../tools/src/hfst-format test.hfst > TMP1 ; then
    exit 1
fi
echo "FOMA_TYPE" > TMP2
if ! diff TMP1 TMP2 ; then
    exit 1
fi
rm test.hfst
rm TMP1
rm TMP2
