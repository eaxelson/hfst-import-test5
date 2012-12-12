#!/bin/sh

./copy-tool-tests.sh

DISTDIR=check-hfst-tools

if [ -d "$DISTDIR" ]; then
    rm -fr $DISTDIR;
fi
mkdir $DISTDIR

cp ./check-tool-tests.sh $DISTDIR/
cp --parents ./tool_tests/* $DISTDIR/

tar -cvf $DISTDIR.tar $DISTDIR
gzip $DISTDIR.tar

rm -fr $DISTDIR
