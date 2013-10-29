#!/bin/sh
#
# Copy directory $1 without the .svn files to directory $2
#

CURRENTDIR=`pwd`

if [ "$1" = "" ]; then
    echo $0": error: source directory must be given as argument"
    exit 1
fi
if [ "$2" = "" ]; then
    echo $0": error: target directory must be given as argument"
    exit 1
fi

SOURCEDIR=$1
TARGETDIR=$2

if ! [ -d $SOURCEDIR ]; then
    echo $0": error: source directory "$SOURCEDIR" does not exist"
    exit 1
fi
if [ -d $TARGETDIR ]; then
    echo $0": error: target directory "$TARGETDIR" already exists"
    exit 1
else
    mkdir $TARGETDIR
fi

if ! (tar -c --exclude=.svn --exclude-backups $SOURCEDIR | tar -x -C $TARGETDIR); then
    echo $0": error: archiving directory "$SOURCEDIR" and extracting it to "$TARGETDIR" failed"
    exit 1
fi

cd $TARGETDIR
mv $SOURCEDIR/* .
cd $CURRENTDIR
rmdir $TARGETDIR/$SOURCEDIR
