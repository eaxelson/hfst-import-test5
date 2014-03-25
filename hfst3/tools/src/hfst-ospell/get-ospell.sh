#!/bin/sh

if [ -x hfst-ospell ]; then rm -fR hfst-ospell; fi
if [ -x src ]; then rm -fR src; fi
svn checkout http://svn.code.sf.net/p/hfst/code/trunk/hfst-ospell

tar -c --exclude=.svn hfst-ospell > tmp
rm -fR hfst-ospell
cat tmp | tar -x
rm tmp
rm hfst-ospell/Makefile.am
cp src_Makefile.am hfst-ospell/Makefile.am
mv hfst-ospell src
