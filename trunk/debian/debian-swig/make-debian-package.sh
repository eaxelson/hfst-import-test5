#!/bin/bash
#
#  -------------------------------------------------
#  A script for creating the debian package for HFST
#  python bindings
#  -------------------------------------------------
#

HFST_PREFIX=`pwd`"/hfst-installation/"
HFST_SWIG=`pwd`"/"

HFST_LIBNUMBER=`ls $HFST_PREFIX/lib/ | egrep 'libhfst\.so\.[0-9]+$' \
    | perl -pe 's/libhfst\.so\.([0-9]+)$/\1/'`


#  -------------------
#  Check control files
#  -------------------

#if grep "Version: ?" debian/DEBIAN/control > /dev/null; then
#    echo "Version number must be defined in control file!";
#    exit 1;
#fi

#if grep "Architecture: ?" debian/DEBIAN/control > /dev/null; then
#    echo "Architecture must be defined in control file!";
#    exit 1;
#fi

#if grep "Provides" debian/DEBIAN/control | grep "?" > /dev/null; then
#    echo "Check provided libraries in control file!";
#    exit 1;
#fi

#if grep "libhfst ?" debian/DEBIAN/shlibs > /dev/null; then
#    echo "Version number must be defined in shlibs file!";
#    exit 1;
#fi


cd python2.7/dist-packages &&
for pyfile in libhfst.py _libhfst.so
do
    cp 
done &&
chmod 0644 * &&
cd ../..

# and duplicate them for python2.6
mkdir python2.6 &&
cd python2.6 &&
mkdir dist-packages &&
cd dist-packages &&
for pyfile in hfst_tagger_compute_data_statistics.py tagger_aux.py
do
    cp ../../python2.7/dist-packages/$pyfile .
done &&
chmod 0644 * &&
cd ../..

cd ../../..


# -----------------------
# Configure the libraries
# -----------------------

#cp debian/DEBIAN/control debian/control
#dpkg-shlibdeps debian/usr/bin/*
#rm debian/control


# -----------------------------------------------------
# gzip the changelog file, if needed, and the man pages
# -----------------------------------------------------

#if [ -e debian/usr/share/doc/hfst-dev/changelog.Debian ]; then
#    gzip --force --best debian/usr/share/doc/hfst-dev/changelog.Debian;
#fi

#gzip --best --force debian/usr/share/man/man1/*.1


# ---------------------------------
# Set file and directory properties
# ---------------------------------

#chmod 0755 debian/DEBIAN/postinst
#chmod 0664 debian/DEBIAN/control
#chmod 0644 debian/DEBIAN/shlibs
#chmod 0644 debian/usr/share/doc/hfst-dev/changelog.Debian.gz
#chmod 0644 debian/usr/share/doc/hfst-dev/copyright

#find . -type d -exec chmod 755 {} \; 


# -----------------------
# Make the debian package
# -----------------------

#fakeroot dpkg-deb --build debian


# --------------------------------------------------------------------
# Test if the package is ok (lintian will probably give some warnings, 
# see if they should be handled...)
# --------------------------------------------------------------------

lintian debian.deb

# change the package name
#PACKAGE_VERSION=`grep 'Version:' ./debian/DEBIAN/control \
#    | perl -pe "s/^Version: ([0-9.\-]+) *$/\1/"`

#ARCHITECTURE=amd64
#if (grep "i386" debian/DEBIAN/control > /dev/null 2> /dev/null); then
#    ARCHITECTURE=i386
#fi

#if test -e debian.deb; then
#    mv debian.deb "hfst-dev_${PACKAGE_VERSION}_${ARCHITECTURE}.deb";
#fi

# unzip the changelog file, so that svn is not confused because it is missing
#gunzip debian/usr/share/doc/hfst-dev/changelog.Debian.gz


# To install the package, execute
# sudo dpkg -i debian.deb
#
# and to remove
# sudo dpkg -r foo-dev
#
# To make a copy without the .svn files
# tar -c --exclude=.svn debian-test | tar -x -C debian-test-copy
# cd debian-test-copy
# mv debian-test/* .
# rmdir debian-test




