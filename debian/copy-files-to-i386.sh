#!/bin/sh

if [ "$1" != "--hfstdir" -o "$2" = "" ]; then
    echo "error: usage: "$0" --hfstdir HFSTDIR"
    exit 1
fi

HFSTDIR=$2

DEBIAN_DIR=/mnt/i386/home/$USER/debian-release

if [ -e $DEBIAN_DIR ]; then
    echo "removing old target directory "$DEBIAN_DIR".."
    sudo rm -fR $DEBIAN_DIR
fi

echo "making directory "$DEBIAN_DIR".."

sudo mkdir $DEBIAN_DIR
sudo mkdir $DEBIAN_DIR/debian/
sudo mkdir $DEBIAN_DIR/debian/debian-test/
sudo mkdir $DEBIAN_DIR/debian/debian-swig/
sudo mkdir $DEBIAN_DIR/debian/$HFSTDIR/

echo "starting copying to "$DEBIAN_DIR"/debian/.."

sudo cp copy-directory.sh $DEBIAN_DIR/debian/
echo "copied copy-directory.sh.."
sudo cp install-hfst-locally.sh $DEBIAN_DIR/debian/
echo "copied install-hfst-locally.sh.."
sudo cp -R debian-test/* $DEBIAN_DIR/debian/debian-test/
echo "copied directory debian-test.."
sudo cp -R debian-swig/* $DEBIAN_DIR/debian/debian-swig/
echo "copied directory debian-swig.."
sudo cp -R $HFSTDIR/* $DEBIAN_DIR/debian/$HFSTDIR/
echo "copied directory "$HFSTDIR".."

sudo sed -i 's/amd64/i386/' $DEBIAN_DIR/debian/debian-test/debian/DEBIAN/control
sudo sed -i 's/amd64/i386/' $DEBIAN_DIR/debian/debian-swig/debian/DEBIAN/control
echo "changed architecture from amd64 to i386 in control files.."
