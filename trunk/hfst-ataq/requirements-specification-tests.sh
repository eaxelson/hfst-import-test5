#!/bin/bash
builddir=.
binary=${builddir}/hfst-ataq

err=0
for t in *.test ; do
    ${binary} < ${t} > ${t%.test}.out
    if ! diff ${t%.test}.out ${t%.test}.expected ; then
        echo "Error in test ${t} (see diffs above)"
        err=1
    fi
done
if test ${err} = 1 ; then
    exit 1;
fi
