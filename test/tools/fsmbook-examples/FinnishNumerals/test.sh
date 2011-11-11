#!/bin/bash

# sfst composition filter..
# foma composition filter and same paths printed many times...
for i in openfst-tropical; do 
  sh $1/FinnishNumerals.hfst.script $i
  hfst-fst2strings $2/EnglishToFinnishNumerals.hfst | uniq | sort > $2/TMP1
  hfst-fst2strings $2/FinnishToEnglishNumerals.hfst | uniq | sort > $2/TMP2
  diff -q $1/english_to_finnish_numerals_expected $2/TMP1 &&
  diff -q $1/finnish_to_english_numerals_expected $2/TMP2;
done
