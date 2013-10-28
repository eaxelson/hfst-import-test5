#!/bin/bash
#
#  -------------------------------------------------
#  A script for creating the debian package for HFST
#  python bindings
#  -------------------------------------------------
#

# -----------------------------------------------------
# Check program arguments and files that it needs exist
# -----------------------------------------------------

if [ "$1" = "--help" -o "$1" = "-h" ]; then
    echo "Usage: make-debian-package.sh --version X.Y.Z --python[2|3]"
    echo "X.Y.Z is hfst version number, [2|3] is python version"
    exit
fi

if ! [ "$1" = "--version" ]; then
    echo "ERROR: you must give version number (with --version X.Y.Z)"
    exit 1
fi
HFST_VERSION=$2

HFST_SWIG_DIR=`pwd`"/../debian-test-copy/hfst-"$HFST_VERSION"/swig"

if ! [ -e "$HFST_SWIG_DIR" ]; then
    echo "ERROR: no directory '"$HFST_SWIG_DIR"' (did you give a valid version number?)"
    exit 1
fi

PYTHON_VERSION=
if [ "$3" = "--python2" ]; then
    if ! [ -e $HFST_SWIG_DIR/python2-libhfst.py ]; then
        echo "ERROR: missing file "$HFST_SWIG_DIR"/python2-libhfst.py"
        exit 1
    fi
    if ! [ -e $HFST_SWIG_DIR/_libhfst.so ]; then
        echo "ERROR: missing file "$HFST_SWIG_DIR"/_libhfst.so"
        exit 1
    fi
    PYTHON_VERSION=2;
elif [ "$3" = "--python3" ]; then
    if ! [ -e $HFST_SWIG_DIR/python3-libhfst.py ]; then
        echo "ERROR: missing file "$HFST_SWIG_DIR"/python3-libhfst.py"
        exit 1
    fi
    if ! [ -e $HFST_SWIG_DIR/_libhfst.cpython-32mu.so ]; then
        echo "ERROR: missing file "$HFST_SWIG_DIR"/_libhfst.cpython-32mu.so"
        exit 1
    fi
    PYTHON_VERSION=3;
else
    echo "ERROR: you must specify python version (with --python[2|3])"
    exit 1
fi


#  ---------------------------
#  Check and edit control file
#  ---------------------------

DEBVERSION=`echo $HFST_VERSION | sed 's/[0-9].\([0-9].[0-9]\)/\1/'`
if ! (grep 'Version: ' debian/DEBIAN/control | grep $DEBVERSION > /dev/null 2> /dev/null); then
    echo "ERROR: wrong version number in control file (field 'Version')"
    exit 1
fi
if ! (grep 'Depends: ' debian/DEBIAN/control | grep $DEBVERSION > /dev/null 2> /dev/null); then
    echo "ERROR: wrong version number in control file (field 'Depends')"
    exit 1
fi

ARCHITECTURE=amd64
if (grep "i386" debian/DEBIAN/control > /dev/null 2> /dev/null); then
    ARCHITECTURE=i386
fi

if [ "$PYTHON_VERSION" = "2" ]; then
    sed -i "s/HFSTPY-DEV/hfstpy2-dev/" debian/DEBIAN/control
    sed -i "s/HFSTPY/hfstpy2/" debian/DEBIAN/control
    sed -i "s/PYTHON_MIN_VERSION/2.6/" debian/DEBIAN/control
    mv debian/usr/share/doc/HFSTPY-DEV debian/usr/share/doc/hfstpy2-dev
    sed -i "s/HFSTPY/hfstpy2/" debian/usr/share/doc/hfstpy2-dev/changelog.Debian
else
    sed -i "s/HFSTPY-DEV/hfstpy3-dev/" debian/DEBIAN/control
    sed -i "s/HFSTPY/hfstpy3/" debian/DEBIAN/control
    sed -i "s/PYTHON_MIN_VERSION/3.2/" debian/DEBIAN/control
    mv debian/usr/share/doc/HFSTPY-DEV debian/usr/share/doc/hfstpy3-dev
    sed -i "s/HFSTPY/hfstpy3/" debian/usr/share/doc/hfstpy3-dev/changelog.Debian
fi


cd debian/usr/lib

if [ "$PYTHON_VERSION" = "2" ]; then
    mkdir python2.7 &&
    chmod 755 python2.7 &&
    cd python2.7 &&
    mkdir dist-packages &&
    chmod 755 dist-packages &&
    cd dist-packages &&
    cp $HFST_SWIG_DIR/python2-libhfst.py libhfst.py &&
    echo '#!/usr/bin/python' >> tmp &&
    cat libhfst.py >> tmp &&
    mv tmp libhfst.py &&
    chmod 755 libhfst.py &&
    cp $HFST_SWIG_DIR/_libhfst.so _libhfst.so &&
    strip _libhfst.so &&
    chmod 755 _libhfst.so &&
    cd ../..
else
    mkdir python3 &&
    chmod 755 python3 &&
    cd python3 &&
    mkdir dist-packages &&
    chmod 755 dist-packages &&
    cd dist-packages &&
    cp $HFST_SWIG_DIR/python3-libhfst.py libhfst.py &&
    echo '#!/usr/bin/python' >> tmp &&
    cat libhfst.py >> tmp &&
    mv tmp libhfst.py &&
    chmod 755 libhfst.py &&
    cp $HFST_SWIG_DIR/_libhfst.cpython-32mu.so _libhfst.so &&
    strip _libhfst.so &&
    chmod 755 _libhfst.so &&
    cd ../..
fi

cd ../../..

if [ -e debian/usr/share/doc/hfstpy-dev/changelog.Debian ]; then
    gzip --force --best debian/usr/share/doc/hfstpy-dev/changelog.Debian
    chmod 644 debian/usr/share/doc/hfstpy-dev/changelog.Debian.gz
fi

fakeroot dpkg-deb --build debian

lintian debian.deb

if test -e debian.deb; then
    if [ "$PYTHON_VERSION" = "2" ]; then
        mv debian.deb "hfstpy2-dev_${DEBVERSION}-1_${ARCHITECTURE}.deb";
    else
        mv debian.deb "hfstpy3-dev_${DEBVERSION}-1_${ARCHITECTURE}.deb";
    fi
fi

# unzip the changelog file, so that svn is not confused because it is missing
gunzip debian/usr/share/doc/hfstpy-dev/changelog.Debian.gz

# delete temporary files

if [ "$PYTHON_VERSION" = "2" ]; then
    rm -r debian/usr/lib/python2.7/* &&
    rmdir debian/usr/lib/python2.7 &&
    mv debian/usr/share/doc/hfstpy2-dev debian/usr/share/doc/HFSTPY-DEV
    sed -i "s/hfstpy3/HFSTPY/" debian/usr/share/doc/HFSTPY-DEV/changelog.Debian
    sed -i "s/hfstpy2-dev/HFSTPY-DEV/" debian/DEBIAN/control
    sed -i "s/hfstpy2/HFSTPY/" debian/DEBIAN/control
    sed -i "s/python (>= 2.6)/python (>= PYTHON_MIN_VERSION)/" debian/DEBIAN/control
else
    rm -r debian/usr/lib/python3/* &&
    rmdir debian/usr/lib/python3 &&
    mv debian/usr/share/doc/hfstpy3-dev debian/usr/share/doc/HFSTPY-DEV
    sed -i "s/hfstpy2/HFSTPY/" debian/usr/share/doc/HFSTPY-DEV/changelog.Debian
    sed -i "s/hfstpy2-dev/HFSTPY-DEV/" debian/DEBIAN/control
    sed -i "s/hfstpy2/HFSTPY/" debian/DEBIAN/control
    sed -i "s/python (>= 3.2)/python (>= PYTHON_MIN_VERSION)/" debian/DEBIAN/control
fi


# To install the package, execute
# sudo dpkg -i debian.deb
#
# and to remove
# sudo dpkg -r foo-dev
