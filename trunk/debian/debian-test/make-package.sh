#!/bin/bash
#
#  -------------------------------------------------
#  A script for creating the debian package for HFST
#  -------------------------------------------------
#

HFST_PREFIX="/home/eaxelson/hfst-installation/"
BACKEND_PREFIX="/usr/local/"
SFST_PREFIX=$BACKEND_PREFIX
OPENFST_PREFIX=$BACKEND_PREFIX
FOMA_PREFIX=$BACKEND_PREFIX

HFST_LIBNUMBER=`ls $HFST_PREFIX/lib/ | egrep 'libhfst\.so\.[0-9]+$' \
    | perl -pe 's/libhfst\.so\.([0-9]+)$/\1/'`

if grep "Version: ?" debian/DEBIAN/control > /dev/null; then
    echo "Version number must be defined in control file!";
    exit 1;
fi

if grep "Architecture: ?" debian/DEBIAN/control > /dev/null; then
    echo "Architecture must be defined in control file!";
    exit 1;
fi

if grep "Provides" debian/DEBIAN/control | grep "?" > /dev/null; then
    echo "Check provided libraries in control file!";
    exit 1;
fi

if grep "libhfst ?" debian/DEBIAN/shlibs > /dev/null; then
    echo "Version number must be defined in shlibs file!";
    exit 1;
fi


for library_mentioned in `cat debian/DEBIAN/shlibs | cut -f1 -d' '`;
do 
    if (! grep "$library_mentioned" debian/DEBIAN/control > /dev/null); then 
	if [ "$library_mentioned" != "libhfst" ]; then
	    echo "ERROR:" $library_mentioned \
		"not found in control file but mentioned in shlibs file";
	    exit 1;
	fi; 
    fi;
done

# -------------------
# Copy the HFST tools
# -------------------

cd debian/usr/bin;

for tool in $HFST_PREFIX/bin/hfst-*;
do
    if (ldd $tool | grep "libhfst\.so\.""$HFST_LIBNUMBER" > /dev/null) ; then
	if ! (echo $tool | grep "2$" > /dev/null) && \
	    ! (echo $tool | grep "3$" > /dev/null) && \
	     ! (echo $tool | grep "\~" > /dev/null) ; then
	    cp -P $tool . ;
	fi
    fi
done

for tool in hfst-twolc hfst-twolc-loc htwolcpre1 htwolcpre2 htwolcpre3;
do
    cp -P $HFST_PREFIX/bin/$tool . ;
done

for tool in hfst-twolc hfst-twolc-loc;
do
    if (egrep "local|home" $tool > /dev/null); then
	echo "ERROR: '"$tool"' uses prefix other than '/usr/bin/'!" ;
	echo "Fix it manually before creating the debian package.";
	exit 1;
    fi;
done

# hfst-xfst depends on foma tools
rm -f hfst-foma-wrapper.sh hfst-xfst 1> /dev/null 2> /dev/null

for tool in hfst-* htwolcpre*;
do
    if (readelf -a $tool 1> /dev/null 2> /dev/null); then
	strip $tool;
    fi;
done
cd ../../..


# ---------------------
# Copy the HFST library
# ---------------------

cd debian/usr/lib/

cp $HFST_PREFIX/lib/libhfst.so."$HFST_LIBNUMBER".0.0 .
ln -s -T libhfst.so."$HFST_LIBNUMBER".0.0 libhfst.so."$HFST_LIBNUMBER"
ln -s -T libhfst.so."$HFST_LIBNUMBER" libhfst.so

#cp -P $HFST_PREFIX/lib/libhfstlexc.so.0.0.0 .
#ln -s -T libhfstlexc.so.0.0.0 libhfstlexc.so.0
#ln -s -T libhfstlexc.so.0 libhfstlexc.so

cp -P $BACKEND_PREFIX/lib/libhfstospell.so.1.0.0 .
ln -s -T libhfstospell.so.1.0.0 libhfstospell.so.1
ln -s -T libhfstospell.so.1 libhfstospell.so

# ---------------------------
# Copy the back-end libraries
# ---------------------------

# OpenFst
cp $BACKEND_PREFIX/lib/libfst.so.0.0.0 .
rm -f libfst.so libfst.so.0
ln -s -T libfst.so.0.0.0 libfst.so.0
ln -s -T libfst.so.0 libfst.so

# SFST
if (grep "Provides" ../../../debian/DEBIAN/control | \
    grep "libsfst" > /dev/null); then
    cp $BACKEND_PREFIX/lib/libsfst.so.0.0.0 .
    rm -f libsfst.so libsfst.so.0
    ln -s -T libsfst.so.0.0.0 libsfst.so.0
    ln -s -T libsfst.so.0 libsfst.so;
fi

#if ! (readelf -a libsfst.so.0.0.0 | grep "SONAME" | \
#      grep "libsfst.so.0" 2>&1 > /dev/null); then
#    echo "ERROR: missing or wrong SONAME in libsfst.so.0.0.0"
#    echo "try recompiling SFST with option '-Wl,-soname,libsfst.so.0'"
#    exit 1;
#fi

# foma
if (grep "Provides" ../../../debian/DEBIAN/control \
    | grep "libfoma" > /dev/null); then
    cp $BACKEND_PREFIX/lib/libfoma.so.0.9.16 .
    rm --force libfoma.so.0 libfoma.so
    ln -s -T libfoma.so.0.9.16 libfoma.so.0
    ln -s -T libfoma.so.0 libfoma.so;
fi

strip *.so
chmod 0644 *

cd ../../..

# --------------------------
# Copy the HFST header files
# --------------------------

cp -PR $HFST_PREFIX/include/hfst/* debian/usr/include/hfst/
chmod 0644 debian/usr/include/*
chmod 0755 debian/usr/include/hfst


# ------------------
# Copy the man pages
# ------------------

for manpage in $HFST_PREFIX/share/man/man1/hfst-*.1; 
do
    if ! (echo $manpage | grep "2\.1" > /dev/null) && \
	! (echo $manpage | grep "3\.1" > /dev/null);
    then
	cp -P $manpage debian/usr/share/man/man1;
    fi
done


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


# ---------------------------------
# Set file and directory properties
# ---------------------------------

chmod 0755 debian/DEBIAN/postinst
chmod 0664 debian/DEBIAN/control
chmod 0644 debian/DEBIAN/shlibs
chmod 0644 debian/usr/share/doc/hfst-dev/changelog.Debian.gz
chmod 0644 debian/usr/share/doc/hfst-dev/copyright

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
