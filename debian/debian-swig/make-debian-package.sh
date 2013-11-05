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
    echo "Usage: make-debian-package.sh --version VER --python[2|3] --hfst-dir PATH"
    echo ""
    echo "VER (N.N.N) is hfst version number, [2|3] is python version,"
    echo "PATH is full path to the directory where hfst is installed"
    exit
fi

if ! [[ "$1" = "--version" && ( "$3" = "--python2" || "$3" = "--python3" ) && "$4" = "--hfst-dir" && "$5" != "" ]]; then
    echo "ERROR: wrong arguments"
    echo ""
    echo "Usage: make-debian-package.sh --version VER --python[2|3] --hfst-dir PATH"
    echo ""
    echo "VER (N.N.N) is hfst version number, [2|3] is python version,"
    echo " PATH is full path to the directory where hfst is installed"
    exit
fi

HFST_VERSION=$2

HFST_SWIG_DIR=$5
if ! [ -e "$HFST_SWIG_DIR" ]; then
    echo "ERROR: no directory '"$HFST_SWIG_DIR"'"
    exit 1
fi

PYTHON_VERSION=2
if [ "$3" = "--python3" ]; then
    PYTHON_VERSION=3
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


# set names and versions of python
if [ "$PYTHON_VERSION" = "2" ]; then
    sed -i "s/HFSTPY-DEV/hfstpy2-dev/" debian/DEBIAN/control
    sed -i "s/HFSTPY/hfstpy2/" debian/DEBIAN/control
    sed -i "s/PYTHON_MIN_VERSION/2.6/" debian/DEBIAN/control
    sed -i "s/PYTHON_NAME/python/" debian/DEBIAN/control
    sed -i "s/HFSTPY/hfstpy2/" debian/usr/share/doc/HFSTPY-DEV/changelog.Debian
    mv debian/usr/share/doc/HFSTPY-DEV debian/usr/share/doc/hfstpy2-dev
    chmod 0755 debian/usr/share/doc/hfstpy2-dev
    chmod 0644 debian/usr/share/doc/hfstpy2-dev/changelog.Debian
else # version 3
    sed -i "s/HFSTPY-DEV/hfstpy3-dev/" debian/DEBIAN/control
    sed -i "s/HFSTPY/hfstpy3/" debian/DEBIAN/control
    sed -i "s/PYTHON_MIN_VERSION/3.2/" debian/DEBIAN/control
    sed -i "s/PYTHON_NAME/python3/" debian/DEBIAN/control
    sed -i "s/HFSTPY/hfstpy3/" debian/usr/share/doc/HFSTPY-DEV/changelog.Debian
    mv debian/usr/share/doc/HFSTPY-DEV debian/usr/share/doc/hfstpy3-dev
    chmod 0755 debian/usr/share/doc/hfstpy3-dev
    chmod 0644 debian/usr/share/doc/hfstpy3-dev/changelog.Debian
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
else # version 3
    mkdir python3 &&
    chmod 755 python3 &&
    cd python3 &&
    mkdir dist-packages &&
    chmod 755 dist-packages &&
    cd dist-packages &&
    cp $HFST_SWIG_DIR/python3-libhfst.py libhfst.py &&
    echo '#!/usr/bin/python3' >> tmp &&
    cat libhfst.py >> tmp &&
    mv tmp libhfst.py &&
    chmod 755 libhfst.py &&
    cp $HFST_SWIG_DIR/_libhfst.cpython-32mu.so _libhfst.so &&
    strip _libhfst.so &&
    chmod 755 _libhfst.so &&
    cd ../..
fi

cd ../../..

if [ -e debian/usr/share/doc/hfstpy2-dev/changelog.Debian ]; then
    gzip --force --best debian/usr/share/doc/hfstpy2-dev/changelog.Debian
    chmod 644 debian/usr/share/doc/hfstpy2-dev/changelog.Debian.gz
fi
if [ -e debian/usr/share/doc/hfstpy3-dev/changelog.Debian ]; then
    gzip --force --best debian/usr/share/doc/hfstpy3-dev/changelog.Debian
    chmod 644 debian/usr/share/doc/hfstpy3-dev/changelog.Debian.gz
fi

fakeroot dpkg-deb --build debian

lintian debian.deb

# name debian package file
if test -e debian.deb; then
    if [ "$PYTHON_VERSION" = "2" ]; then
        mv debian.deb "hfstpy2-dev_${DEBVERSION}-1_${ARCHITECTURE}.deb";
    else
        mv debian.deb "hfstpy3-dev_${DEBVERSION}-1_${ARCHITECTURE}.deb";
    fi
fi

# delete temporary files

if [ "$PYTHON_VERSION" = "2" ]; then
    rm -r debian/usr/lib/python2.7/* &&
    rmdir debian/usr/lib/python2.7 &&
    # unzip the changelog file, so that svn is not confused because it is missing
    gunzip debian/usr/share/doc/hfstpy2-dev/changelog.Debian.gz
    sed -i "s/hfstpy2/HFSTPY/" debian/usr/share/doc/hfstpy2-dev/changelog.Debian
    mv debian/usr/share/doc/hfstpy2-dev debian/usr/share/doc/HFSTPY-DEV
    sed -i "s/hfstpy2-dev/HFSTPY-DEV/" debian/DEBIAN/control
    sed -i "s/hfstpy2/HFSTPY/" debian/DEBIAN/control
    sed -i "s/, python/, PYTHON_NAME/" debian/DEBIAN/control
    sed -i "s/PYTHON_NAME (>= 2.6)/PYTHON_NAME (>= PYTHON_MIN_VERSION)/" debian/DEBIAN/control
else
    rm -r debian/usr/lib/python3/* &&
    rmdir debian/usr/lib/python3 &&
    # unzip the changelog file, so that svn is not confused because it is missing
    gunzip debian/usr/share/doc/hfstpy3-dev/changelog.Debian.gz
    sed -i "s/hfstpy3/HFSTPY/" debian/usr/share/doc/hfstpy3-dev/changelog.Debian
    mv debian/usr/share/doc/hfstpy3-dev debian/usr/share/doc/HFSTPY-DEV
    sed -i "s/hfstpy3-dev/HFSTPY-DEV/" debian/DEBIAN/control
    sed -i "s/hfstpy3/HFSTPY/" debian/DEBIAN/control
    sed -i "s/, python3/, PYTHON_NAME/" debian/DEBIAN/control
    sed -i "s/PYTHON_NAME (>= 3.2)/PYTHON_NAME (>= PYTHON_MIN_VERSION)/" debian/DEBIAN/control
fi

# To install the package, execute
# sudo dpkg -i debian.deb
#
# and to remove
# sudo dpkg -r foo-dev
