#!/bin/bash

for i in EsperantoAdjectives \
      	 EsperantoNounsAndAdjectives \
	 EsperantoNounsAndAdjectivesWithTags \
	 EsperantoNouns \
         EsperantoNounsAdjectivesAndVerbs;
do
  # compile with xfst
#  echo 'read lexc < "'$i'.lexc"; write att "'$i'" FOO BAR;' | xfst #\
#      2>1 > /dev/null;
#  cat $i | ../xfst-att-to-hfst-att.sh > $i.xfst.att;
  cat $1/$i.xfst.att | hfst-txt2fst -f openfst-tropical > $2/$i.xfst.hfst;

  # compile with different HFST backends
  for j in sfst openfst-tropical foma;
  do
    bash $1/$i.hfst.script $j;
    # test for equivalence
    hfst-fst2fst -f openfst-tropical $2/$i.hfst.hfst > $2/TMP;
    if ! hfst-compare -q $2/TMP $2/$i.xfst.hfst; then
      echo "FAIL:" $i "with" $j "failed!";
      exit 1;
    fi;
  done;

done
