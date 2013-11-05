#!/bin/bash
#
#  -------------------------------------------------
#  A script for creating the debian package for HFST
#  command line tools and API.
#  -------------------------------------------------
#

if [ "$1" != "--hfst-dir" -o "$2" = "" ]; then
    echo $0": error: directory where hfst is installed must be given"
    echo $0" --hfst-dir DIRNAME"
    exit 1
fi
HFST_PREFIX=$2

HFST_LIBNUMBER=`ls $HFST_PREFIX/lib/ | egrep 'libhfst\.so\.[0-9]+$' \
    | perl -pe 's/libhfst\.so\.([0-9]+)$/\1/'`


#  -------------------
#  Check control files
#  -------------------

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
    # copy all tools that are linked to libhfst library
    # omit older versions
    if (ldd $tool | grep "libhfst\.so\.""$HFST_LIBNUMBER" > /dev/null) ; then
	if ! (echo $tool | grep "2$" > /dev/null) && \
	    ! (echo $tool | grep "3$" > /dev/null) && \
	     ! (echo $tool | grep "\~" > /dev/null) ; then
	    cp -P $tool . ;
	fi
    fi
done

#copy hfst-train-tagger script
cp -P $HFST_PREFIX/bin/hfst-train-tagger .

# copy hfst-twolc scripts and executables that it needs
for tool in hfst-twolc htwolcpre1 htwolcpre2 htwolcpre3;
do
    cp -P $HFST_PREFIX/bin/$tool . ;
done

# remove local version of hfst-twolc and hfst-train-tagger-loc
if test -e $HFST_PREFIX/bin/hfst-twolc-loc; then
    rm -f $HFST_PREFIX/bin/hfst-twolc-loc;
fi
if test -e $HFST_PREFIX/bin/hfst-train-tagger-loc; then
    rm -f $HFST_PREFIX/bin/hfst-train-tagger-loc;
fi

# replace prefix /usr/local/bin/ with /usr/bin/
for tool in hfst-twolc;
do
    sed -i 's/usr\/local\/bin\//usr\/bin\//' $tool;
done

# TODO: replace local paths with /usr/ in hfst-twolc?

# foma wrapper depends on zlib..
rm -f hfst_foma hfst-foma-wrapper.sh hfst-foma-wrapper 1> /dev/null 2> /dev/null

# strip tools
for tool in hfst-* htwolcpre*;
do
    if ! [ "$tool" = "hfst-train-tagger" ]; then
        if (readelf -a $tool 1> /dev/null 2> /dev/null); then
	    strip $tool;
        fi;
    fi;
done
cd ../../..


# ---------------------
# Copy the HFST library
# ---------------------

cd debian/usr/lib/

cp $HFST_PREFIX/lib/libhfst.so."$HFST_LIBNUMBER".0.0 .
chrpath -d libhfst.so."$HFST_LIBNUMBER".0.0
ln -s -T libhfst.so."$HFST_LIBNUMBER".0.0 libhfst.so."$HFST_LIBNUMBER"
ln -s -T libhfst.so."$HFST_LIBNUMBER" libhfst.so

strip *.so
chmod 0644 *

# also copy tagger python scripts
PY_SRC_DIR=
PY_TARGET_DIRS=
if [ -e "$HFST_PREFIX/lib/python2.7" ]; then
    PY_SRC_DIR="python2.7"
    PY_TARGET_DIRS="python2.7 python2.6"
elif [ -e "$HFST_PREFIX/lib/python2.6" ]; then
    PY_SRC_DIR="python2.6"
    PY_TARGET_DIRS="python2.6"
else
    echo "ERROR: tagger python scripts not found"
    exit 1
fi

for py_target_dir in $PY_TARGET_DIRS
do
    mkdir $py_target_dir &&
    cd $py_target_dir &&
    mkdir dist-packages &&
    cd dist-packages &&
    for pyfile in hfst_tagger_compute_data_statistics.py tagger_aux.py
    do
        if [ -e "$HFST_PREFIX/lib/"$PY_SRC_DIR"/site-packages" ]; then
            cp $HFST_PREFIX/lib/$PY_SRC_DIR/site-packages/$pyfile .
        elif [ -e "$HFST_PREFIX/lib/"$PY_SRC_DIR"/dist-packages" ]; then
            cp $HFST_PREFIX/lib/$PY_SRC_DIR/dist-packages/$pyfile .
        else
            echo "ERROR: tagger python scripts not found"
            exit 1
        fi
    done &&
    chmod 0644 * &&
    cd ../..
done

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
    # omit older versions
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
    # hfst-twolc is a script, skip it
    if [ "$program" != "hfst-twolc" ]; then
	if ! [ -L "$program" ]; then 
	    chrpath -d $program ; 
	fi; 
    fi
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

# change the package name
PACKAGE_VERSION=`grep 'Version:' ./debian/DEBIAN/control \
    | perl -pe "s/^Version: ([0-9.\-]+) *$/\1/"`

ARCHITECTURE=amd64
if (grep "i386" debian/DEBIAN/control > /dev/null 2> /dev/null); then
    ARCHITECTURE=i386
fi

if test -e debian.deb; then
    mv debian.deb "hfst-dev_${PACKAGE_VERSION}_${ARCHITECTURE}.deb";
fi

# unzip the changelog file, so that svn is not confused because it is missing
gunzip debian/usr/share/doc/hfst-dev/changelog.Debian.gz


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




