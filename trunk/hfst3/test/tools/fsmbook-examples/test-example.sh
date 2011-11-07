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


# # If $1.xfst.att does not exist, compile it.
# if ! [ -f $1.xfst.att]; then
#   xfst -f $1.xfst.script;
#   cat $1 | ./xfst-att-to-hfst-att.sh > $1.xfst.att;
# fi


# Convert the AT&T format into an HFST transducer in format openfst-tropical.
  hfst-txt2fst -f openfst-tropical $1.xfst.att > $2.xfst.hfst

# For all HFST implementation types,
  for i in sfst openfst-tropical foma; do

    # echo "Testing" $i "..."
  # run the HFST script using the implementation type,
    sh $1.hfst.script $i
  # convert the result into openfst-tropical type
    hfst-fst2fst -f openfst-tropical $2.hfst.hfst > TMP
  # and compare it with the expected result.
    hfst-compare -q TMP $2.xfst.hfst # && echo "TEST PASSED";

  done
