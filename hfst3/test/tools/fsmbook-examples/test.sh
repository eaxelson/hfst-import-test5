#!/bin/bash

# Perform check in each directory.
# Rules must be implemented before all directories can be tested. 
# Directory Lingala requires a professional licence of xfst so that
# the resulting transducer can be written in att format. 

# EinsteinsPuzzle Palindromes
# FinnishNumerals NumbersToNumerals

for i in DateParser EnglishNumerals \
; do \
cd $i; 
if !(../kokeilu.sh $i); then
    exit 1
fi
cd ..; 
done

# We usually have in each file "Foo"
# - an xfst script "Foo.xfst.script" that is basically the xfst file found
#   in the fsmbook web page
# - either (1) a ready-compiled AT&T format representation "Foo.xfst.att" or 
#   (2) a string (pair) representation "expected_strings" of the transducer 
#   that is obtained when executing the script "Foo.xfst.script" 
# - an HFST script "Foo.hfst.script" that is compiled into a transducer
#   using all HFST implementation types.
# - a Makefile "Makefile.am"
# - a test file test.script that is executed by "Makefile.am" when make check
#   is done
# 
# For each HFST implementation type, we test if the result from 
# "Foo.hfst.script" is equivalent to "Foo.xfst.att" or "expected_strings".
