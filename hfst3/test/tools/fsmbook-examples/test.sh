#!/bin/bash
# Rules must be implemented before FinnishProsody can be tested 
for i in DateParser EinsteinsPuzzle EnglishNumerals \
FinnishNumerals NumbersToNumerals Palindroms; do \
cd $i; 
if !(make check); then
    exit 1
fi
cd ..; 
done
