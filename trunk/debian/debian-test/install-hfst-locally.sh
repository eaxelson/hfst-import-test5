#!/bin/sh

HFST_INSTDIR=hfst-installation
HFST_INSTDIR_FULL_PATH=`pwd`/$HFST_INSTDIR

HFST_FULL_PATH=`pwd`/../../hfst3
# Get newest version of HFST tarball
HFST_VERSION=`ls $HFST_FULL_PATH/hfst-*.tar.gz | sort | tail -1 | sed -e 's/.*\(hfst-[0-9]\+.[0-9]\+.[0-9]\+\).*/\1/'`
TAR_EXT=.tar.gz

# Remove old local installations of HFST
for dir in $HFST_INSTDIR $HFST_VERSION;
do
    if [ -x $dir ]; then
	rm -fR $dir
    fi
    mkdir $dir;
done

# Copy and extract the newest HFST tarball
cp $HFST_FULL_PATH/$HFST_VERSION$TAR_EXT . && \
tar -xzvf $HFST_VERSION$TAR_EXT && \
cd $HFST_VERSION

# Make HFST and install it locally
autoreconf -i && \
./configure --with-foma --prefix=$HFST_INSTDIR_FULL_PATH \
--enable-calculate --enable-edit-metadata --enable-lexc \
--enable-tagger --enable-proc --enable-shuffle && \
make && make install
