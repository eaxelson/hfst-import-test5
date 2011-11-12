#!/bin/bash

# hfst-xfst -f NumbersToNumerals.xfst.script
# rm FOO BAR
# cat NumbersToNumerals | ../xfst-att-to-hfst-att.sh \
# > NumbersToNumerals.xfst.att

hfst-txt2fst -f openfst-tropical $1/NumbersToNumerals.xfst.att > \
  $2/NumbersToNumerals.xfst.hfst
hfst-project -p input $2/NumbersToNumerals.xfst.hfst > $2/expected_input
hfst-project -p output $2/NumbersToNumerals.xfst.hfst > $2/expected_output

for i in sfst openfst-tropical foma; do
  sh $1/NumbersToNumerals.hfst.script $i
  hfst-project -p input $2/NumbersToNumerals.hfst.hfst \
  | hfst-fst2fst -f openfst-tropical > $2/result_input
  hfst-project -p output $2/NumbersToNumerals.hfst.hfst \
  | hfst-fst2fst -f openfst-tropical > $2/result_output
  hfst-compare -q $2/expected_input $2/result_input && \
  hfst-compare -q $2/expected_input $2/result_input;
done
