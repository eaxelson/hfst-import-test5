#!/bin/sh
#
# Copy HFST dll, command line tools and swig bindings to the current directory
#

# (1) HFST dll file

HFST_DLL_DIRECTORY=../libhfst/src/.libs
HFST_DLL=libhfst-36.dll
cp $HFST_DLL_DIRECTORY/$HFST_DLL .
strip $HFST_DLL

# (2) dependency dll files

DEPENDENCY_DLL_DIRECTORY=/mingw/bin/
DEPENDENCY_DLLS="libgcc_s_dw2-1.dll libgcc_s_seh-1.dll libstdc++-6.dll \
libbz2-2.dll libiconv-2.dll liblzma-5.dll zlib1.dll"
for dll in $DEPENDENCY_DLLS;
do
    if (test -e $DEPENDENCY_DLL_DIRECTORY/$dll); then
        cp $DEPENDENCY_DLL_DIRECTORY/$dll .;
    fi
done

# (3) Python bindings

#SWIG_DIRECTORY=../SWIG/
#SWIG_FILES="_libhfst.pyd libhfst.py"

#for swigfile in $SWIG_FILES;
#do
#    cp $SWIG_DIRECTORY/$swigfile .;
#done

# (4) tagger tools

#TAGGER_SCRIPT_DIRECTORY=../tools/src/hfst-tagger/src/
#TAGGER_SCRIPT="hfst-train-tagger.bat"
#TAGGER_BINARY_DIRECTORY=../tools/src/hfst-tagger/src/.libs/
#TAGGER_BINARIES="hfst-open-input-file-for-tagger.exe hfst-build-tagger.exe hfst-tag.exe"
#TAGGER_PYTHON_FILES="../tools/src/hfst-tagger/src/hfst_tagger_compute_data_statistics.py  \
#../tools/src/hfst-tagger/src/build_model_src/tagger_aux.py"

#cp $TAGGER_SCRIPT_DIRECTORY/$TAGGER_SCRIPT .
#for binary in $TAGGER_BINARIES;
#do
#    cp $TAGGER_BINARY_DIRECTORY/$binary .
#    strip $binary;
#done
#for pyfile in $TAGGER_PYTHON_FILES;
#do
#    cp $pyfile .;
#done

# (5) twolc tools

TWOLC_SCRIPT_DIRECTORY=../tools/src/hfst-twolc/src/
TWOLC_SCRIPT="hfst-twolc.bat"
TWOLC_BINARY_DIRECTORY=../tools/src/hfst-twolc/src/.libs/
TWOLC_BINARIES="htwolcpre1.exe htwolcpre2.exe htwolcpre3.exe"

cp $TWOLC_SCRIPT_DIRECTORY/$TWOLC_SCRIPT .
for binary in $TWOLC_BINARIES;
do
    cp $TWOLC_BINARY_DIRECTORY/$binary .
    strip $binary;
done

# (6) XFST tool

XFST_DIRECTORY=../tools/src/parsers/.libs/
XFST_BINARY=hfst-xfst.exe
cp $XFST_DIRECTORY/$XFST_BINARY .
strip $XFST_BINARY

# (7) proc tool

PROC_DIRECTORY=../tools/src/hfst-proc/.libs/
PROC_BINARY=hfst-apertium-proc.exe
PROC_NAME=hfst-proc.exe
cp $PROC_DIRECTORY/$PROC_BINARY $PROC_NAME
strip $PROC_NAME

# (8) other tools

TOOL_DIRECTORY=../tools/src/.libs/
TOOLS="hfst-affix-guessify.exe \
hfst-calculate.exe \
hfst-compare.exe \
hfst-compose.exe \
hfst-compose-intersect.exe \
hfst-concatenate.exe \
hfst-conjunct.exe \
hfst-determinize.exe \
hfst-disjunct.exe \
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
hfst-lookup.exe \
hfst-pair-test.exe \
hfst-minimize.exe \
hfst-multiply.exe \
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
hfst-txt2fst.exe"

for tool in $TOOLS; 
do
    cp $TOOL_DIRECTORY/$tool .
    strip $tool;
done
