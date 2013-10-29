#!/bin/sh
#
# A script for installing hfst (including swig bindings)
# locally so that they can be used in making a debian package.
#

HFST_INSTDIR=hfst-installation
HFST_INSTDIR_FULL_PATH=`pwd`/$HFST_INSTDIR

HFST_FULL_PATH=`pwd`/../../hfst3
# Get newest version number from HFST tarball
#
HFST_VERSION=`ls $HFST_FULL_PATH/hfst-*.tar.gz | sort | tail -1 | sed -e 's/.*\(hfst-[0-9]\+.[0-9]\+.[0-9]\+\).*/\1/'`
TAR_EXT=.tar.gz

# Remove old local installations of HFST
#
for dir in $HFST_INSTDIR $HFST_VERSION;
do
    if [ -x $dir ]; then
	rm -fR $dir
    fi
    mkdir $dir;
done

# Copy and extract the newest HFST tarball
#
cp $HFST_FULL_PATH/$HFST_VERSION$TAR_EXT . && \
tar -xzvf $HFST_VERSION$TAR_EXT && \
cd $HFST_VERSION

# Make HFST and install it locally
#
# add linker flags -L/usr/lib/ -Wl,-rpath=/usr/lib/
# so that /usr/local/lib/libhfst.so will not be used
#
autoreconf -i && \
./configure --with-foma --prefix=$HFST_INSTDIR_FULL_PATH \
--with-sfst=no LDFLAGS="-L/usr/lib/ -Wl,-rpath=/usr/lib/" \
--enable-calculate --enable-edit-metadata --enable-lexc \
--enable-tagger --enable-proc --enable-shuffle \
--enable-xfst --enable-foma-wrapper --with-readline && \
./scripts/generate-cc-files.sh && make && make install

# Make swig bindings
#
# add linker flags -L/usr/lib/ -Wl,-rpath=/usr/lib/
# so that /usr/local/lib/libhfst.so will not be used
#
if [ "$1" = "--with-swig" ]; then
    cd swig
    sed -i 's/extra_link_args = \[\]/extra_link_args = \["-L\/usr\/lib\/", "-Wl,-rpath=\/usr\/lib\/"\]/' setup.py
    touch libhfst.i
    python setup.py build_ext --inplace
    mv libhfst.py python2-libhfst.py
    touch libhfst.i
    python3 setup.py build_ext --inplace
    mv libhfst.py python3-libhfst.py
    cd ..
fi
