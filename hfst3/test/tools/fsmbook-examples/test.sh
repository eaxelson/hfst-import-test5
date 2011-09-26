#!/bin/bash
# Rules must be implemented before directories FinnishProsody, PlusOrMinus
# and FinnishOTProsody can be tested 
# Directory Lingala requires a professional licence of xfst so that
# the resulting transducer can be written in att format. 
for i in DateParser EinsteinsPuzzle EnglishNumerals \
FinnishNumerals NumbersToNumerals Palindroms; do \
cd $i; 
if !(make check); then
    exit 1
fi
cd ..; 
done
