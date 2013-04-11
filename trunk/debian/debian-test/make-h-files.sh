#!/bin/sh

cd libhfst/src/parsers/
make xre_parse.cc
make pmatch_parse.cc
make lexc-parser.cc
mv xre_parse.hh xre_parse.h
mv pmatch_parse.hh pmatch_parse.h
mv lexc-parser.hh lexc-parser.h
cd ../../..
cd tools/src
make hfst-compiler.cc
mv hfst-compiler.hh hfst-compiler.h
cd hfst-twolc/src/
make htwolcpre1.cc
mv htwolcpre1.hh htwolcpre1.h
mv htwolcpre2.hh htwolcpre2.h
mv htwolcpre3.hh htwolcpre3.h
cd ../../../..