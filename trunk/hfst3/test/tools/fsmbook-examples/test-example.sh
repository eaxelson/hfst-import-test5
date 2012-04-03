#!/bin/bash

# Perform check in a directory specified by $1.
#
# This program assumes that in directory $1 there are files
#
#  (1) $1.xfst.att: A file in AT&T format specifying the expected result.
#                   The file is ready-compiled with xfst from the file
#                   $1.xfst.script.
#
#  (2) $1.hfst.script: A file using HFST commandline tools to achieve
#                      the same result as yielded by xfst from $1.xfst.script.
#                      The file stores its result in a file named $1.hfst.hfst.


if [ "$4" = "--full-test" ] ; then
# If $1.xfst.att does not exist, compile it.
if ! [ -f $1.xfst.att ]; then
  xfst -f $1.xfst.script;
  cat $1 | ../xfst-att-to-hfst-att.sh > $1.xfst.att;
  rm $1;
fi


# If $1.foma.att does not exist, compile it.
if ! [ -f $1.foma.att ]; then
  foma -f $1.xfst.script 2>1 > /dev/null;
  cat $1" FOO BAR" | ../foma-att-to-hfst-att.sh > $1.foma.att;
  rm $1" FOO BAR";
fi
fi




# Convert the AT&T format into an HFST transducer in format openfst-tropical.
  $3/hfst-txt2fst -f openfst-tropical $1.xfst.att > $2.xfst.hfst

if [ "$4" = "--full-test" ] ; then 
  $3/hfst-txt2fst -f openfst-tropical $1.foma.att > $2.foma.hfst
  if ! ( $3/hfst-compare -q $2.foma.hfst $2.xfst.hfst ); then
    exit 1;
  fi
fi



# For all HFST implementation types,
  for i in openfst-tropical sfst foma; do

    echo "Testing" $i "..."
    if ! ($3/hfst-format --test-format $i); then
   continue;
    fi

  # run the HFST script using the implementation type,
    sh $1.hfst.script $i $3
  # convert the result into openfst-tropical type
   $3/hfst-fst2txt $2.hfst.hfst > TMP.att
    $3/hfst-txt2fst -f openfst-tropical TMP.att > TMP
  # and compare it with the expected result.
    if ! ( $3/hfst-compare -q TMP $2.xfst.hfst ); then
   echo "The result is incorrect!"
   exit 1;
    fi
   
   done


