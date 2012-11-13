#!/bin/bash
TOOLDIR=../../tools/src

# sfst composition filter..
# foma composition filter and same paths printed many times...
for i in openfst-tropical; do 

  if ! ($3/$TOOLDIR/hfst-format --test-format $i); then
      continue;
  fi


  sh $1/FinnishNumerals.hfst.script $i $3
  $3/$TOOLDIR/hfst-fst2strings $2/EnglishToFinnishNumerals.hfst | uniq | sort > $2/TMP1
  $3/$TOOLDIR/hfst-fst2strings $2/FinnishToEnglishNumerals.hfst | uniq | sort > $2/TMP2
  cat $1/english_to_finnish_numerals_expected | sort > $2/TMP3
  cat $1/finnish_to_english_numerals_expected | sort > $2/TMP4
  diff -q $2/TMP3 $2/TMP1 &&
  diff -q $2/TMP4 $2/TMP2;
done
