#!/bin/sh
# This script performs all the flex/bison compilation needed
# when compiling HFST. It is intended to be run before 'make'
# so that all generated header files will get the right extension.

cd libhfst/src/parsers/
rm -f xre_parse.h pmatch_parse.h lexc-parser.h
make xre_parse.cc
make pmatch_parse.cc
make lexc-parser.cc
test -e xre_parse.h && mv xre_parse.h xre_parse.hh
test -e pmatch_parse.h && mv pmatch_parse.h pmatch_parse.hh
test -e lexc-parser.h && mv lexc-parser.h lexc-parser.hh
cd ../../..
cd tools/src
rm -f hfst-compiler.h
make hfst-compiler.cc
test -e hfst-compiler.h && mv hfst-compiler.h hfst-compiler.hh
cd hfst-twolc/src/
rm -f htwolcpre1.h htwolcpre2.h htwolcpre3.h
make htwolcpre1.cc
make htwolcpre2.cc
make htwolcpre3.cc
test -e htwolcpre1.h && mv htwolcpre1.h htwolcpre1.hh
test -e htwolcpre2.h && mv htwolcpre2.h htwolcpre2.hh
test -e htwolcpre3.h && mv htwolcpre3.h htwolcpre3.hh
cd ../../../..
