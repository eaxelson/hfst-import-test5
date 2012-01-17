#!/bin/bash

if [ "$4" = "--test-full" ]; then

  # If $1.foma.att does not exist, compile it.
    #if ! [ -f $1/EinsteinsPuzzle.foma.att ]; then
	foma -f $1/EinsteinsPuzzle.xfst.script 2>1 > /dev/null;
	cat $1/EinsteinsPuzzle | ../foma-att-to-hfst-att.sh \
	    > $1/EinsteinsPuzzle.foma.att;
	rm $1/EinsteinsPuzzle;
    #fi

  # Test that the result is correct
    $3/hfst-txt2fst -f openfst-tropical $1/EinsteinsPuzzle.foma.att \
	| $3/hfst-fst2strings -S > TMP;

    if ! (diff -q TMP $1/expected_result_foma); then
	exit 1;
    fi
    
fi


for i in sfst openfst-tropical foma; do

  if ! ($3/hfst-format --test-format $i); then
      continue;
  fi

  bash $1/EinsteinsPuzzle.hfst.script $i $3

 if ! ( diff -q $2/result $1/expected_result ); then
     exit 1;
 fi

done
