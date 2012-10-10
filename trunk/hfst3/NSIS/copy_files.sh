#!/bin/sh
#
# Copy HFST dll, command line tools and swig bindings to the current directory
#

LIBDIR=../libhfst/src/.libs/
DLLDIR=/mingw/bin/
SWIGDIR=../SWIG/
TOOLDIR=../tools/src/.libs/

HFST_LIB=libhfst-21.dll
DEPENDENCY_DLLS="libgcc_s_dw2-1.dll libstdc++-6.dll"
SWIG_FILES="_libhfst.pyd libhfst.py"

TOOLS="hfst-affix-guessify.exe \
hfst-calculate.exe \
hfst-compare.exe \
hfst-compose.exe \
hfst-compose-intersect.exe \
hfst-concatenate.exe \
hfst-conjunct.exe \
hfst-determinize.exe \
hfst-disjunct.exe \
hfst-duplicate.exe \
hfst-edit-metadata.exe \
hfst-format.exe \
hfst-fst2fst.exe \
hfst-fst2strings.exe \
hfst-fst2txt.exe \
hfst-grep.exe \
hfst-guess.exe \
hfst-guessify.exe \
hfst-head.exe \
hfst-info.exe \
hfst-invert.exe \
hfst-lexc.exe \
hfst-lexc2fst.exe \
hfst-lookup.exe \
hfst-pair-test.exe \
hfst-minimize.exe \
hfst-name.exe \
hfst-optimized-lookup.exe \
hfst-pmatch.exe \
hfst-pmatch2fst.exe \
hfst-project.exe \
hfst-push-weights.exe \
hfst-regexp2fst.exe \
hfst-remove-epsilons.exe \
hfst-repeat.exe \
hfst-reverse.exe \
hfst-reweight.exe \
hfst-shuffle.exe \
hfst-split.exe \
hfst-strings2fst.exe \
hfst-substitute.exe \
hfst-subtract.exe \
hfst-summarize.exe \
hfst-tail.exe \
hfst-traverse.exe \
hfst-txt2fst.exe"
# hfst-train-tagger.exe hfst-twolc.exe hfst-expand-equivalences.exe


cp $LIBDIR/$HFST_LIB .
strip $HFST_LIB

for dependency_dll in $DEPENDENCY_DLLS;
do
    cp $DLLDIR/$dependency_dll .;
done

for tool in $TOOLS; 
do
    cp $TOOLDIR/$tool .;
    strip $tool;
done

for swigfile in $SWIG_FILES;
do
    cp $SWIGDIR/$swigfile .;
done
