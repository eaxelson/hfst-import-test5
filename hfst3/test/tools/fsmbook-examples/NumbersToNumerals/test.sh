#!/bin/bash

# hfst-xfst -f NumbersToNumerals.xfst.script
# rm FOO BAR
# cat NumbersToNumerals | ../xfst-att-to-hfst-att.sh \
# > NumbersToNumerals.xfst.att

$3/hfst-txt2fst -f openfst-tropical $1/NumbersToNumerals.xfst.att > \
  $2/NumbersToNumerals.xfst.hfst
$3/hfst-project -p input $2/NumbersToNumerals.xfst.hfst > $2/expected_input
$3/hfst-project -p output $2/NumbersToNumerals.xfst.hfst > $2/expected_output

for i in sfst openfst-tropical foma; do

  if ! ($3/hfst-format --test-format $i); then
      continue;
  fi

  sh $1/NumbersToNumerals.hfst.script $i $3
  $3/hfst-project -p input $2/NumbersToNumerals.hfst.hfst \
  | $3/hfst-fst2fst -f openfst-tropical > $2/result_input
  $3/hfst-project -p output $2/NumbersToNumerals.hfst.hfst \
  | $3/hfst-fst2fst -f openfst-tropical > $2/result_output
  $3/hfst-compare -q $2/expected_input $2/result_input && \
  $3/hfst-compare -q $2/expected_input $2/result_input;
done
