#!/bin/sh

#
# Make a distribution tarball check-hfst-tools[-VERSIONNMUBER].tar.gz that
# contains tools for testing installed hfst tools.
#

VERSION=
if [ "$1" = "-h" -o "$1" = "--help" ]; then
    echo ""
    echo "  Usage:  "$0" [--version VERSIONNUMBER]"
    echo ""
    exit 0;
elif [ "$1" = "--version" ]; then
    VERSION=$2;
fi


./copy-tool-tests.sh

if [ ! "$VERSION" = "" ]; then
    DISTDIR=check-hfst-tools"-"$VERSION;
else
    DISTDIR=check-hfst-tools
fi

if [ -d "$DISTDIR" ]; then
    rm -fr $DISTDIR;
fi
mkdir $DISTDIR

if [ ! -d "tool_tests" ]; then
    echo "ERROR: directory 'tool_tests' does not exist."
    exit 1;
fi

cp dist-README $DISTDIR/README
cp ./check-tool-tests.sh $DISTDIR/
cp --parents ./tool_tests/* $DISTDIR/

tar -cvf $DISTDIR.tar $DISTDIR
gzip $DISTDIR.tar

rm -fr $DISTDIR

