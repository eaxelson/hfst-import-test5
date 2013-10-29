#!/bin/sh

if [ "$1" = "" ]; then
    echo $0": error: version number must be given as argument"
    exit 1
fi
VERSION=$1

DEBIAN_DIR=/mnt/i386/home/$USER/debian-release

if [ -e $DEBIAN_DIR ]; then
    echo "removing old target directory "$DEBIAN_DIR
    sudo rm -fR $DEBIAN_DIR
fi

echo "making directory "$DEBIAN_DIR

sudo mkdir $DEBIAN_DIR
sudo mkdir $DEBIAN_DIR/hfst3/
sudo mkdir $DEBIAN_DIR/debian/
sudo mkdir $DEBIAN_DIR/debian/debian-test/
sudo mkdir $DEBIAN_DIR/debian/debian-swig/

echo "starting copying to "$DEBIAN_DIR

sudo cp ../hfst3/hfst-$VERSION.tar.gz $DEBIAN_DIR/hfst3/
echo "copied ../hfst3/hfst-"$VERSION".tar.gz"
sudo cp -R debian-test/* $DEBIAN_DIR/debian/debian-test/
echo "copied debian-test"
sudo cp -R debian-swig/* $DEBIAN_DIR/debian/debian-swig/
echo "copied debian-swig"

sudo sed -i 's/amd64/i386/' $DEBIAN_DIR/debian/debian-test/debian/DEBIAN/control
sudo sed -i 's/amd64/i386/' $DEBIAN_DIR/debian/debian-swig/debian/DEBIAN/control
