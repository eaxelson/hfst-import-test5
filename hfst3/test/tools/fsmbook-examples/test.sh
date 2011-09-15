#!/bin/bash
for i in DateParser EinsteinsPuzzle EnglishNumerals \
FinnishNumerals NumbersToNumerals; do \
cd $i; 
if !(make check); then
    exit 1
fi
cd ..; 
done
