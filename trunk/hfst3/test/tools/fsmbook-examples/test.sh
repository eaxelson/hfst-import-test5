#!/bin/bash
for i in DateParser EinsteinsPuzzle EnglishNumerals \
FinnishNumerals NumbersToNumerals; do \
cd $i; make check; cd ..; done
