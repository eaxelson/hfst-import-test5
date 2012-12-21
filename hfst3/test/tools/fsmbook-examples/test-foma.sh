#!/bin/bash
TOOLDIR=../../tools/src

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


# create foma result
if ! [ -f $1.foma.hfst ]; then
    foma -f $1.xfst.script;
    gunzip -f $1.foma.gz
    $3/$TOOLDIR/hfst-fst2fst -f openfst-tropical $1.foma -o $1.foma.hfst
fi

# For all HFST implementation types,
  for i in sfst openfst-tropical foma; do
    echo "Testing" $i "..."
    if ! ($3/$TOOLDIR/hfst-format --test-format $i); then
        continue;
    fi

     # run the HFST script using the implementation type,
    sh $1.hfst.script $i $3
    # convert result to openfst-tropical type transducer
    $3/$TOOLDIR/hfst-fst2fst -f openfst-tropical $1.hfst.hfst > TMP

    # and compare it with the expected result.
    if ! ( $3/$TOOLDIR/hfst-compare -q TMP $1.foma.hfst ); then
        echo "The result is incorrect!"
        exit 1;
    fi
   
   done


