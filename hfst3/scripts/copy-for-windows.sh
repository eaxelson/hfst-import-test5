#!/bin/sh

# A script for copying winopenfst (top directory given as the second argument) 
# and foma backends and libhfst/src in a directory given as first argument
# for native windows compilation.

if [ -d "$1" ]; then
    echo "Directory $1 exists"
    exit 1
else
    mkdir $1
fi

#openfstdir=
#if [ "$2" = "" ]; then
    openfstdir="back-ends/openfstwin"
#    echo "No second argument given, defaulting to openfst directory '$openfstdir'"
#else
#    openfstdir=$2
#fi

mkdir $1/back-ends

# foma back-end
mkdir $1/back-ends/foma/

for file in \
apply.c coaccessible.c constructions.c \
define.c determinize.c dynarray.c \
extract.c flags.c int_stack.c io.c \
lex.cmatrix.c lex.yy.c mem.c minimize.c \
regex.c reverse.c rewrite.c sigma.c \
spelling.c stringhash.c structures.c \
topsort.c trie.c utf8.c foma.h fomalib.h \
fomalibconf.h regex.h;
do 
    cp back-ends/foma/$file $1/back-ends/foma/;
done

# openfstwin back-end
mkdir $1/back-ends/openfstwin
mkdir $1/back-ends/openfstwin/src
mkdir $1/back-ends/openfstwin/src/include
mkdir $1/back-ends/openfstwin/src/include/fst
mkdir $1/back-ends/openfstwin/src/lib

for file in \
compat flags fst properties symbol-table symbol-table-ops util; 
do
    cp $openfstdir/src/lib/$file.cc $1/back-ends/openfstwin/src/lib/$file.cpp 
done

# file symbol-table-ops.cc not in version 1.2.6
#if [ -f "$openfstdir/src/lib/symbol-table-ops.cc" ]; then
#    cp $openfstdir/src/lib/symbol-table-ops.cc $1/back-ends/openfstwin/src/lib/symbol-table-ops.cpp
#fi

cp $openfstdir/src/include/fst/*.h $1/back-ends/openfstwin/src/include/fst/

# libhfst/src and subdirectories
mkdir $1/libhfst
mkdir $1/libhfst/src
mkdir $1/libhfst/src/implementations
mkdir $1/libhfst/src/implementations/compose_intersect
mkdir $1/libhfst/src/implementations/optimized-lookup
mkdir $1/libhfst/src/parsers

# libhfst/src without subdirectories
for file in \
FormatSpecifiers.h HarmonizeUnknownAndIdentitySymbols.h \
HfstDataTypes.h HfstEpsilonHandler.h HfstExceptionDefs.h \
HfstExceptions.h HfstExtractStrings.h HfstFlagDiacritics.h \
HfstInputStream.h HfstLookupFlagDiacritics.h HfstOutputStream.h \
HfstSymbolDefs.h HfstTokenizer.h HfstTransducer.h HfstXeroxRules.h \
hfst.h hfst.hpp.in hfst_apply_schemas.h hfstdll.h;
do
    cp libhfst/src/$file $1/libhfst/src/
done

for file in \
HarmonizeUnknownAndIdentitySymbols HfstApply HfstDataTypes \
HfstEpsilonHandler HfstExceptionDefs HfstExceptions HfstFlagDiacritics \
HfstInputStream HfstLookupFlagDiacritics HfstOutputStream HfstRules \
HfstSymbolDefs HfstTokenizer HfstTransducer HfstXeroxRules \
HfstXeroxRulesTest;
do
    cp libhfst/src/$file.cc $1/libhfst/src/$file.cpp
done

# libhfst/src/implementations without subdirectories
for file in \
ConvertTransducerFormat.h FomaTransducer.h HfstFastTransitionData.h \
HfstOlTransducer.h HfstTransition.h HfstTransitionGraph.h \
HfstTropicalTransducerTransitionData.h LogWeightTransducer.h \
TropicalWeightTransducer.h;
do
    cp libhfst/src/implementations/$file $1/libhfst/src/implementations/
done

for file in \
ConvertFomaTransducer ConvertLogWeightTransducer ConvertOlTransducer \
ConvertTransducerFormat ConvertTropicalWeightTransducer FomaTransducer \
HfstOlTransducer HfstTransitionGraph HfstTropicalTransducerTransitionData \
LogWeightTransducer TropicalWeightTransducer;
do
    cp libhfst/src/implementations/$file.cc $1/libhfst/src/implementations/$file.cpp
done

# implementations/compose_intersect
for file in \
ComposeIntersectFst ComposeIntersectLexicon ComposeIntersectRule \
ComposeIntersectRulePair ComposeIntersectUtilities;
do
    cp libhfst/src/implementations/compose_intersect/$file.cc \
        $1/libhfst/src/implementations/compose_intersect/$file.cpp
    cp libhfst/src/implementations/compose_intersect/$file.h \
        $1/libhfst/src/implementations/compose_intersect/$file.h
done

# implementations/optimized-lookup
for file in \
convert pmatch transducer;
do
    cp libhfst/src/implementations/optimized-lookup/$file.cc \
        $1/libhfst/src/implementations/optimized-lookup/$file.cpp
    cp libhfst/src/implementations/optimized-lookup/$file.h \
        $1/libhfst/src/implementations/optimized-lookup/$file.h
done
for file in \
find_epsilon_loops ospell;
do
    cp libhfst/src/implementations/optimized-lookup/$file.cc \
        $1/libhfst/src/implementations/optimized-lookup/$file.cpp
done

# parsers
for file in \
LexcCompiler PmatchCompiler XreCompiler \
lexc-utils pmatch_utils xre_utils;
do
    cp libhfst/src/parsers/$file.cc \
        $1/libhfst/src/parsers/$file.cpp
    cp libhfst/src/parsers/$file.h \
        $1/libhfst/src/parsers/$file.h
done
for file in \
lexc-lexer pmatch_lex xre_lex;
do
    cp libhfst/src/parsers/$file.cc \
        $1/libhfst/src/parsers/$file.cpp
done
for file in \
lexc-parser pmatch_parse xre_parse;
do
    cp libhfst/src/parsers/$file.cc \
        $1/libhfst/src/parsers/$file.cpp
    cp libhfst/src/parsers/$file.hh \
        $1/libhfst/src/parsers/$file.hh
done

# make scripts and headers
# cp scripts/make-foma.bat $1/back-ends/foma/
# cp scripts/make-openfstwin.bat $1/back-ends/openfstwin/src/lib/
# cp scripts/test-openfstwin.bat $1/back-ends/openfstwin/src/lib/
# cp scripts/make-parsers.bat $1/libhfst/src/parsers/
# cp scripts/make-implementations.bat $1/libhfst/src/implementations/
# cp scripts/make-libhfst.bat $1/libhfst/src/
# cp scripts/test-libhfst.bat $1/libhfst/src/
# cp scripts/generate-python-bindings.bat $1/libhfst/src/
cp scripts/make-python-bindings.bat $1/libhfst/src/
cp scripts/test_libhfst_win.py $1/libhfst/src/
cp scripts/libhfst_win.i $1/libhfst/src/
cp scripts/make-hfst-xfst.bat $1/libhfst/src/
cp scripts/make-hfst-proc.bat $1/libhfst/src/
cp scripts/make-hfst-lexc.bat $1/libhfst/src/
cp scripts/make-hfst-tool.bat $1/libhfst/src/
cp scripts/make-hfst-tools.bat $1/libhfst/src/
cp scripts/make-htwolcpre1.bat $1/libhfst/src/
cp scripts/make-htwolcpre2.bat $1/libhfst/src/
cp scripts/make-htwolcpre3.bat $1/libhfst/src/

for file in test.lexc test.pmatch test.twolc test.xfst \
test_xfst_result.txt test_twolc_result.txt test_pmatch_result.txt test_lexc_result.txt;
do
    cp scripts/windows_tests/$file $1/libhfst/src/
done

cp scripts/test-hfst-tools.bat $1/libhfst/src/
cp scripts/README_xfst_win.txt $1/libhfst/src/
cp scripts/README_eight_tools_win.txt $1/libhfst/src/

# copy missing headers and change some headers included
cp scripts/stdint.h $1/back-ends/foma/
cp scripts/inttypes.h $1/back-ends/foma/
# unistd.h is included in some generated files
for file in \
$1/libhfst/src/parsers/pmatch_lex.cpp \
$1/libhfst/src/parsers/xre_lex.cpp \
$1/libhfst/src/parsers/lexc-lexer.cpp \
$1/back-ends/foma/lex.cmatrix.c;
do
    sed -i 's/#include <unistd.h>/#include <io.h>/' $file
done
sed -i 's/pmatchwrap( )/pmatchwrap(void)/' $1/libhfst/src/parsers/pmatch_lex.cpp
sed -i 's/hlexcwrap( )/hlexcwrap(void)/' $1/libhfst/src/parsers/lexc-lexer.cpp

# copy files for tools

# create subdirectories
mkdir $1/tools
mkdir $1/tools/src
mkdir $1/tools/src/parsers
mkdir $1/tools/src/hfst-proc
mkdir $1/tools/src/inc
mkdir $1/tools/src/hfst-twolc
mkdir $1/tools/src/hfst-twolc/src
mkdir $1/tools/src/hfst-twolc/src/alphabet_src
mkdir $1/tools/src/hfst-twolc/src/commandline_src
mkdir $1/tools/src/hfst-twolc/src/io_src
mkdir $1/tools/src/hfst-twolc/src/rule_src
mkdir $1/tools/src/hfst-twolc/src/string_src
mkdir $1/tools/src/hfst-twolc/src/variable_src


for file in \
xfst-utils XfstCompiler hfst-xfst xfst-parser xfst-lexer \
init_help  xfst_help_message;
do
    cp tools/src/parsers/$file.cc $1/tools/src/parsers/$file.cpp
done

cp tools/src/parsers/xfst_help_message.h $1/tools/src/parsers/

sed -i 's/#include <unistd.h>/#include <io.h>/' $1/tools/src/parsers/xfst-lexer.cpp
sed -i 's/hxfstwrap( )/hxfstwrap(void)/' $1/tools/src/parsers/xfst-lexer.cpp
#sed -i 's/#include "help_message.cc"/#include "help_message.cpp"/' $1/tools/src/parsers/XfstCompiler.cpp


# compare, strings2fst and txt2fst are needed for testing hfst-xfst
for file in \
hfst-program-options hfst-commandline hfst-tool-metadata HfstStrings2FstTokenizer \
hfst-file-to-mem hfst-string-conversions hfst-getopt \
hfst-lexc-compiler hfst-compare hfst-strings2fst hfst-txt2fst hfst-pmatch hfst-pmatch2fst \
hfst-lookup hfst-optimized-lookup;
do
    cp tools/src/$file.cc $1/tools/src/$file.cpp
done

# hfst-proc
for file in \
hfst-proc formatter lookup-path lookup-state tokenizer transducer applicators alphabet;
do
    cp tools/src/hfst-proc/$file.cc $1/tools/src/hfst-proc/$file.cpp
done

for file in \
hfst-proc.h formatter.h lookup-path.h lookup-state.h tokenizer.h \
transducer.h buffer.h applicators.h alphabet.h;
do
    cp tools/src/hfst-proc/$file $1/tools/src/hfst-proc/
done

for file in \
hfst-commandline.h hfst-program-options.h hfst-tool-metadata.h \
HfstStrings2FstTokenizer.h hfst-string-conversions.h \
hfst-file-to-mem.h hfst-getopt.h hfst-optimized-lookup.h;
do
    cp tools/src/$file $1/tools/src/
done

for file in \
XfstCompiler.h xfst-utils.h xfst-parser.hh cmd.h abbrcmd.h;
do
    cp tools/src/parsers/$file $1/tools/src/parsers/
done

for file in \
check-params-binary.h check-params-common.h check-params-unary.h getopt-cases-binary.h \
getopt-cases-common.h getopt-cases-error.h getopt-cases-unary.h globals-binary.h \
globals-common.h globals-unary.h;
do
    cp tools/src/inc/$file $1/tools/src/inc/
done


# Copy twolc

TWOLC_DIR=tools/src/hfst-twolc/src

for file in \
HfstTwolcDefs.h common_globals.h grammar_defs.h hfst-twolc.bat \
htwolcpre1.hh htwolcpre2.hh htwolcpre3.hh;
do
    cp $TWOLC_DIR/$file $1/$TWOLC_DIR/
done

for file in \
hfst-twolc-system htwolcpre1 htwolcpre2 htwolcpre3 scanner1 scanner2 scanner3;
do
    cp $TWOLC_DIR/$file.cc $1/$TWOLC_DIR/$file.cpp
done

for file in scanner1.cpp scanner2.cpp scanner3.cpp;
do
    sed -i 's/#include <unistd.h>/#include <io.h>/' $1/$TWOLC_DIR/$file
    sed -i 's/yywrap( )/yywrap(void)/' $1/$TWOLC_DIR/$file
done

cp $TWOLC_DIR/alphabet_src/Alphabet.cc $1/$TWOLC_DIR/alphabet_src/Alphabet.cpp
cp $TWOLC_DIR/alphabet_src/Alphabet.h $1/$TWOLC_DIR/alphabet_src/Alphabet.h
cp $TWOLC_DIR/commandline_src/CommandLine.cc $1/$TWOLC_DIR/commandline_src/CommandLine.cpp
cp $TWOLC_DIR/commandline_src/CommandLine.h $1/$TWOLC_DIR/commandline_src/CommandLine.h
cp $TWOLC_DIR/io_src/InputReader.cc $1/$TWOLC_DIR/io_src/InputReader.cpp
cp $TWOLC_DIR/io_src/InputReader.h $1/$TWOLC_DIR/io_src/InputReader.h
cp $TWOLC_DIR/io_src/input_defs.h $1/$TWOLC_DIR/io_src/input_defs.h

for file in ConflictResolvingLeftArrowRule ConflictResolvingRightArrowRule \
LeftArrowRule LeftArrowRuleContainer LeftRestrictionArrowRule OtherSymbolTransducer \
RightArrowRule RightArrowRuleContainer Rule RuleContainer TwolCGrammar;
do
    cp $TWOLC_DIR/rule_src/$file.h $1/$TWOLC_DIR/rule_src/$file.h
    cp $TWOLC_DIR/rule_src/$file.cc $1/$TWOLC_DIR/rule_src/$file.cpp
done

cp $TWOLC_DIR/string_src/string_manipulation.cc $1/$TWOLC_DIR/string_src/string_manipulation.cpp
cp $TWOLC_DIR/string_src/string_manipulation.h $1/$TWOLC_DIR/string_src/string_manipulation.h

for file in ConstContainerIterator.h MatchedConstContainerIterator.h MixedConstContainerIterator.h \
RuleSymbolVector.h RuleVariables.h RuleVariablesConstIterator.h VariableBlock.h VariableBlockContainer.h \
VariableContainer.h VariableContainerBase.h VariableDefs.h VariableValueIterator.h VariableValues.h;
do
    cp $TWOLC_DIR/variable_src/$file $1/$TWOLC_DIR/variable_src/
done

for file in RuleSymbolVector RuleVariables RuleVariablesConstIterator VariableValues;
do
    cp $TWOLC_DIR/variable_src/$file.cc $1/$TWOLC_DIR/variable_src/$file.cpp
done

# For convenience
cp $TWOLC_DIR/hfst-twolc.bat $1/libhfst/src/
