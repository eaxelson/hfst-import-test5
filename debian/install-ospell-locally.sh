#!/bin/sh
#
# A script for installing ospell locally so that it can be used
# in making a debian package.
#

ORIG_DIR=`pwd`

if [ "$1" = "--help" -o "$1" = "-h" ]; then
    echo "usage: "$0" --ospell-srcdir SRCDIR --ospell-instdir INSTDIR"
    exit
fi
if [ "$1" != "--ospell-srcdir" -o "$3" != "--ospell-instdir" -o "$4" = "" ]; then
    echo "error: usage: "$0" --ospell-scrdir SRCDIR --ospell-instdir INSTDIR"
    exit 1
fi

OSPELL_SRCDIR=$2
OSPELL_INSTDIR=$4
OSPELL_INSTDIR_FULL_PATH=`pwd`"/"$OSPELL_INSTDIR

if ! [ -e $OSPELL_SRCDIR ]; then
    echo "error: no such source directory: "$OSPELL_SRCDIR
    exit 1
fi
if [ -e $OSPELL_INSTDIR_FULL_PATH ]; then
    echo "error: installation directory "$OSPELL_INSTDIR_FULL_PATH" exists"
    exit 1
fi

mkdir $OSPELL_INSTDIR_FULL_PATH
cd $OSPELL_SRCDIR

# Make OSPELL and install it locally
#
# add linker flags -L/usr/lib/ -Wl,-rpath=/usr/lib/
# so that /usr/local/lib/libospell.so will not be used
#
autoreconf -i && \
./configure --prefix=$OSPELL_INSTDIR_FULL_PATH --enable-xml=tinyxml \
&& make LDFLAGS="-L/usr/lib/ -Wl,-rpath=/usr/lib/" && make install

cd $ORIG_DIR
