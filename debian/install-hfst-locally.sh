#!/bin/sh
#
# A script for installing hfst (including swig bindings)
# locally so that they can be used in making a debian package.
#

ORIG_DIR=`pwd`

if [ "$1" = "--help" -o "$1" = "-h" ]; then
    echo "usage: "$0" --hfst-srcdir SRCDIR --hfst-instdir INSTDIR"
    exit
fi
if [ "$1" != "--hfst-srcdir" -o "$3" != "--hfst-instdir" -o "$4" = "" ]; then
    echo "error: usage: "$0" --hfst-scrdir SRCDIR --hfst-instdir INSTDIR"
    exit 1
fi

HFST_SRCDIR=$2
HFST_INSTDIR=$4
HFST_INSTDIR_FULL_PATH=`pwd`"/"$HFST_INSTDIR

if ! [ -e $HFST_SRCDIR ]; then
    echo "error: no such source directory: "$HFST_SRCDIR
    exit 1
fi
if [ -e $HFST_INSTDIR_FULL_PATH ]; then
    echo "error: installation directory "$HFST_INSTDIR_FULL_PATH" exists"
    exit 1
fi

mkdir $HFST_INSTDIR_FULL_PATH
cd $HFST_SRCDIR

# Make HFST and install it locally
#
# add linker flags -L/usr/lib/ -Wl,-rpath=/usr/lib/
# so that /usr/local/lib/libhfst.so will not be used
#
autoreconf -i && \
./configure --with-foma --with-sfst=no --prefix=$HFST_INSTDIR_FULL_PATH \
--with-sfst=no LDFLAGS="-L/usr/lib/ -Wl,-rpath=/usr/lib/" \
--enable-all-tools --disable-foma-wrapper --disable-lexc-wrapper \
--enable-fsmbook-tests --with-readline=no && \
make && make install

# Make swig bindings
#
# add linker flags -L/usr/lib/ -Wl,-rpath=/usr/lib/
# so that /usr/local/lib/libhfst.so will not be used
#
cd swig
sed -i 's/extra_link_args = \[\]/extra_link_args = \["-L\/usr\/lib\/", "-Wl,-rpath=\/usr\/lib\/"\]/' setup.py
touch libhfst.i
python setup.py build_ext --inplace
mv libhfst.py python2-libhfst.py
mv _libhfst.so python2-_libhfst.so
if (which python3 1> /dev/null 2> /dev/null); then
    touch libhfst.i
    python3 setup.py build_ext --inplace
    mv libhfst.py python3-libhfst.py
    if [ -e _libhfst.so ]; then
        mv _libhfst.so python3-_libhfst.so
    fi
    if [ -e _libhfst.cpython-32mu.so ]; then
        mv _libhfst.cpython-32mu.so python3-_libhfst.so
    fi
    if [ -e _libhfst.cpython-32m.so ]; then
        mv _libhfst.cpython-32m.so python3-_libhfst.so
    fi
fi

cd $ORIG_DIR
