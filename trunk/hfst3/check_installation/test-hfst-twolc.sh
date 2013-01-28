#!/bin/sh
#
# Test that the installed version of hfst-twolc works.
#

if ! (which hfst-twolc 2>1 > /dev/null); then
    echo "warning: hfst-twolc not found, assumed switched off and skipping it"
    exit 0
fi

rm -f twolc.script
echo 'Alphabet a b ;' > twolc.script
echo 'Rules' >> twolc.script
echo '"rule name"' >> twolc.script
echo 'a:b => b _ b ;' >> twolc.script

for format in sfst openfst-tropical foma;
do
    # We just test that the tool is installed right, not its result..
    if ! (cat twolc.script | hfst-twolc -s -f $format 2>1 > /dev/null); then
	exit 1
    fi
done

rm -f twolc.script

