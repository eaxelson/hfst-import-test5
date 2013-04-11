#!/bin/bash

rm flags.txt

i=$1

echo $i;
cd $i/src/morphology


FILE=tmp_flags.txt_list.txt

hfst-summarize -v $i.lexc.hfst | grep -ow "\@[^_]*\@" | sed 's/\, /\n/g' | sort | uniq > $FILE

cat $FILE | sed -e 's/^@[^.]*\.//' | sed -e 's/\.[^@]*@//' | sed -e 's/\@//' | sed '/^$/d' | sort | uniq | sort >> ../../../flags.txt

rm $FILE 
   
cd ../../../

#cat flags.txt

