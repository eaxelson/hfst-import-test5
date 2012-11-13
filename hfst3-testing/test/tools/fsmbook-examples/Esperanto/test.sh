#!/bin/bash

for i in EsperantoAdjectives \
      	 EsperantoNounsAndAdjectives \
	 EsperantoNounsAndAdjectivesWithTags \
	 EsperantoNouns \
         EsperantoNounsAdjectivesAndVerbs;
do

if [ "$4" = "--test-full" ]; then 
# compile with xfst
    echo 'read lexc < "'$i'.lexc"; write att "'$i'" FOO BAR;' | xfst \
      2>1 > /dev/null;
    cat $i | ../xfst-att-to-hfst-att.sh > $i.xfst.att;
fi

  cat $1/$i.xfst.att | $3/hfst-txt2fst -f openfst-tropical > $2/$i.xfst.hfst;

if [ "$4" = "--test-full" ]; then 
# compile with foma
    echo 'read lexc < "'$i'.lexc"; write att "'$i'";' | foma \
      2>1 > /dev/null;
    cat $i | ../foma-att-to-hfst-att.sh > $i.foma.att;

    cat $1/$i.foma.att | $3/hfst-txt2fst -f openfst-tropical > $2/$i.foma.hfst;

    if ! $3/hfst-compare -q $2/$i.xfst.hfst $2/$i.foma.hfst; then
	exit 1;
    fi
fi

  # compile with different HFST backends
  for j in sfst openfst-tropical foma;
  do

    if ! ($3/hfst-format --test-format $i); then
        continue;
    fi

    bash $1/$i.hfst.script $j $3;
    # test for equivalence
    $3/hfst-fst2fst -f openfst-tropical $2/$i.hfst.hfst > $2/TMP;
    if ! $3/hfst-compare -q $2/TMP $2/$i.xfst.hfst; then
      echo "FAIL:" $i "with" $j "failed!";
      exit 1;
    fi;
  done;

done
