#!/bin/bash

for i in sfst openfst-tropical foma; do

  if ! ($3/hfst-format --test-format $i); then
      continue;
  fi

  bash $1/EinsteinsPuzzle.hfst.script $i $3
  diff -q $2/result $1/expected_result;
done
