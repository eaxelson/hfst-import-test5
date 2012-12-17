#!/bin/sh

#
# Make a distribution tarball check-hfst[-VERSIONNMUBER].tar.gz that
# contains tools for testing installed hfst tools and morphologies.
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

echo "  Copying command line tool tests..."
./copy-tool-tests.sh
echo "  Copying tests for swig bindings..."
./copy-swig-tests.sh

if [ ! "$VERSION" = "" ]; then
    DISTDIR=check-hfst"-"$VERSION;
else
    DISTDIR=check-hfst
fi

if [ -d "$DISTDIR" ]; then
    rm -fr $DISTDIR;
fi
mkdir $DISTDIR

for dir in tool_tests swig_tests morphology_tests;
do
    if [ ! -d $dir ]; then
	echo "ERROR: directory 'tool_tests' does not exist."
	exit 1;
    fi
done

cp dist-README $DISTDIR/README
cp ./check-tools.sh $DISTDIR/
cp --parents ./tool_tests/* $DISTDIR/
cp ./check-swig.sh $DISTDIR/
cp --parents ./swig_tests/* $DISTDIR/
cp ./check-morphologies.sh $DISTDIR/
cp --parents ./morphology_tests/* $DISTDIR/

echo "  Creating the package..." 
tar -cvf $DISTDIR.tar $DISTDIR
gzip $DISTDIR.tar

rm -fr $DISTDIR
echo "  Distribution package "$DISTDIR".tar.gz"" created."
