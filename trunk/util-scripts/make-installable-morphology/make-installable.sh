#!/bin/bash

if [ -e hfst-${1}-installable ] ; then
    echo "Target directory already exists"; exit 1
fi
if [ $# -lt 2 ] ; then
    echo "usage: $0 langname langcode [w]"
    exit 1
elif [ $# -lt 3 ] ; then
    fst2fstcommand="hfst-fst2fst -O"
elif [ "$3" = "w" ] ; then
    fst2fstcommand="hfst-fst2fst -w"
else
    echo "Didn't understand argument $3"
    exit 1
fi
set -x
mkdir hfst-${1}-installable
hfst-invert ${1}.hfst | $fst2fstcommand > \
    hfst-${1}-installable/${2}-generation.hfst.ol
$fst2fstcommand ${1}.hfst > \
    hfst-${1}-installable/${2}-analysis.hfst.ol
sed s/LANGCODE/${2}/ < analyze.sh > hfst-${1}-installable/${1}-analyze.sh
sed s/LANGCODE/${2}/ < generate.sh > hfst-${1}-installable/${1}-generate.sh
sed -e s/LANGNAME/${1}/ -e s/LANGCODE/${2}/ < Makefile-skeleton > \
    hfst-${1}-installable/Makefile
