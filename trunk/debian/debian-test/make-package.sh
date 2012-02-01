#!/bin/bash
#
#  -------------------------------------------------
#  A script for creating the debian package for HFST
#  -------------------------------------------------
#

HFST_PREFIX="$HOME/hfst-installation/"
BACKEND_PREFIX="/usr/local/"

# -------------------
# Copy the HFST tools
# -------------------

for tool in $HFST_PREFIX/bin/hfst-*;
do
    cp -P $tool debian/usr/bin/;
done

cd debian/usr/bin;
for tool in hfst-*;
do
    if (readelf -a $tool 2>&1 > /dev/null); then
	strip $tool;
    fi;
done
cd ../../..


# ---------------------
# Copy the HFST library
# ---------------------

cp -P $HFST_PREFIX/lib/* debian/usr/lib/


# ---------------------------
# Copy the back-end libraries
# ---------------------------

cd debian/usr/lib/

# OpenFst
cp $BACKEND_PREFIX/lib/libfst.so.0.0.0 .
rm -f libfst.so libfst.so.0
ln -s -T libfst.so.0.0.0 libfst.so.0
ln -s -T libfst.so.0 libfst.so

# SFST
cp $BACKEND_PREFIX/lib/libsfst.so.0.0.0 .
rm -f libsfst.so libsfst.so.0
ln -s -T libsfst.so.0.0.0 libsfst.so.0
ln -s -T libsfst.so.0 libsfst.so

if ! (readelf -a libsfst.so.0.0.0 | grep "SONAME" | \
      grep "libsfst.so.0" 2>&1 > /dev/null); then
    echo "ERROR: missing or wrong SONAME in libsfst.so.0.0.0"
    echo "try recompiling SFST with option '-Wl,-soname,libsfst.so.0'"
    exit 1;
fi

# foma
cp $BACKEND_PREFIX/lib/libfoma.so.0.9.16 .
rm --force libfoma.so.0 libfoma.so
ln -s -T libfoma.so.0.9.16 libfoma.so.0
ln -s -T libfoma.so.0 libfoma.so

strip *.so
chmod 0644 *

cd ../../..

# --------------------------
# Copy the HFST header files
# --------------------------

cp -PR $HFST_PREFIX/include/hfst/* debian/usr/include/hfst/
chmod 0644 debian/usr/include/*
chmod 0755 debian/usr/include/hfst


# ---------------------------------
# Remove the rpaths from HFST tools
# ---------------------------------

cd debian/usr/bin
for program in *; 
do  
    if ! [ -L "$program" ]; then 
	chrpath -d $program ; 
    fi; 
done
cd ../../..


# -----------------------
# Configure the libraries
# -----------------------

cp debian/DEBIAN/control debian/control
dpkg-shlibdeps debian/usr/bin/*
rm debian/control


# -----------------------------------------------------
# gzip the changelog file, if needed, and the man pages
# -----------------------------------------------------

if [ -e debian/usr/share/doc/hfst-dev/changelog.Debian ]; then
    gzip --force --best debian/usr/share/doc/hfst-dev/changelog.Debian;
fi

gzip --best --force debian/usr/share/man/man1/*.1


echo "Exiting..."
exit 0;
echo "Oops, didn't exit..."


# -----------------------
# Make the debian package
# -----------------------

fakeroot dpkg-deb --build debian


# --------------------------------------------------------------------
# Test if the package is ok (lintian will probably give some warnings, 
# see if they should be handled...)
# --------------------------------------------------------------------

lintian debian.deb


# unzip the changelog file, so that svn is not confused because it is missing
gunzip debian/usr/share/doc/hfst-dev/changelog.Debian.gz

# To install the package, execute
# sudo dpkg -i debian.deb

# and to remove
# sudo dpkg -r foo-dev

# To make a copy without the .svn files
# tar -c --exclude=.svn debian-test | tar -x -C debian-test-copy
# cd debian-test-copy
# mv debian-test/* .
# rmdir debian-test