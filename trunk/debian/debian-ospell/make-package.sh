#!/bin/bash
#
#  -------------------------------------------------
#  A script for creating the debian package for HFST
#  ospell command line tool and API.
#  -------------------------------------------------
#


if [ "$1" != "--hfst-dir" -o "$2" = "" ]; then
    echo $0": error: directory where ospell is installed must be given"
    echo $0" --ospell-dir DIRNAME"
    exit 1
fi
OSPELL_PREFIX=$2

OSPELL_LIBNUMBER=`ls $OSPELL_PREFIX/lib/ | egrep 'libhfstospell\.so\.[0-9]+$' \
    | perl -pe 's/libhfstospell\.so\.([0-9]+)$/\1/'`


# --------------------
# Copy the ospell tool
# --------------------

cd debian/usr/bin;

# copy hfst-ospell
cp -P $OSPELL_PREFIX/bin/hfst-ospell .

# strip tool
for tool in hfst-ospell;
do
    if (readelf -a $tool 1> /dev/null 2> /dev/null); then
        strip $tool;
    fi;
done
cd ../../..


# -----------------------
# Copy the ospell library
# -----------------------

cd debian/usr/lib/

cp -P $OSPELL_PREFIX/lib/libhfstospell.so."$OSPELL_LIBNUMBER".0.0 .
chrpath -d libhfstospell.so."$OSPELL_LIBNUMBER".0.0
ln -s -T libhfstospell.so."$OSPELL_LIBNUMBER".0.0 libhfstospell.so."$OSPELL_LIBNUMBER"
ln -s -T libhfstospell.so."$OSPELL_LIBNUMBER" libhfstospell.so

cp -P $OSPELL_PREFIX/lib/libhfstospell.a .
cp -P $OSPELL_PREFIX/lib/libhfstospell.la .

strip *.so
chmod 0644 *

cd ../../..

# ----------------------------
# Copy the ospell header files
# ----------------------------

cp -PR $OSPELL_PREFIX/include/* debian/usr/include/
chmod 0644 debian/usr/include/*

# -----------------
# Copy the man page
# -----------------

for manpage in $OSPELL_PREFIX/share/man/man1/hfst-ospell.1; 
do
    cp -P $manpage debian/usr/share/man/man1;    
done

# -----------------------
# Copy the pkgconfig file
# -----------------------

cp $OSPELL_PREFIX/lib/pkgconfig/hfstospell.pc debian/usr/lib/pkgconfig/
chmod 0644 debian/usr/lib/pkgconfig/*

# ---------------------------
# Remove the rpaths from tool
# ---------------------------

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
for tool in debian/usr/bin/*;
do
    dpkg-shlibdeps $tool
done
rm debian/control


# -----------------------------------------------------
# gzip the changelog file, if needed, and the man pages
# -----------------------------------------------------

if [ -e debian/usr/share/doc/hfstospell-dev/changelog.Debian ]; then
    gzip --force --best debian/usr/share/doc/hfstospell-dev/changelog.Debian;
fi

gzip --best --force debian/usr/share/man/man1/*.1


# ---------------------------------
# Set file and directory properties
# ---------------------------------

chmod 0755 debian/DEBIAN/postinst
chmod 0664 debian/DEBIAN/control
chmod 0644 debian/DEBIAN/shlibs
chmod 0644 debian/usr/share/doc/hfstospell-dev/changelog.Debian.gz
chmod 0644 debian/usr/share/doc/hfstospell-dev/copyright

find . -type d -exec chmod 755 {} \; 


# -----------------------
# Make the debian package
# -----------------------

fakeroot dpkg-deb --build debian


# --------------------------------------------------------------------
# Test if the package is ok (lintian will probably give some warnings, 
# see if they should be handled...)
# --------------------------------------------------------------------

lintian debian.deb

# change the package name
PACKAGE_VERSION=`grep 'Version:' ./debian/DEBIAN/control \
    | perl -pe "s/^Version: ([0-9.\-]+) *$/\1/"`

ARCHITECTURE=amd64
if (grep "i386" debian/DEBIAN/control > /dev/null 2> /dev/null); then
    ARCHITECTURE=i386
fi

if test -e debian.deb; then
    mv debian.deb "hfstospell-dev_${PACKAGE_VERSION}_${ARCHITECTURE}.deb";
fi

# unzip the changelog file, so that svn is not confused because it is missing
gunzip debian/usr/share/doc/hfstospell-dev/changelog.Debian.gz


# To install the package, execute
# sudo dpkg -i debian.deb
#
# and to remove
# sudo dpkg -r foo-dev

