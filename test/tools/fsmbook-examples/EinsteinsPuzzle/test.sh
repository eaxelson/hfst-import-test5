#!/bin/bash

for i in sfst openfst-tropical foma; do
  bash $1/EinsteinsPuzzle.hfst.script $i
  diff -q $2/result $1/expected_result;
done
