#!/bin/sh

DEBIAN_DIR=/mnt/i386/home/$USER/debian-release

if [ -x $DEBIAN_DIR ]; then
    sudo rm -fR $DEBIAN_DIR
fi

sudo mkdir $DEBIAN_DIR
sudo mkdir $DEBIAN_DIR/hfst3/
sudo mkdir $DEBIAN_DIR/debian/
sudo mkdir $DEBIAN_DIR/debian/debian-test/
sudo mkdir $DEBIAN_DIR/debian/debian-swig/

sudo cp ../../hfst3/hfst-*.tar.gz $DEBIAN_DIR/hfst3/
sudo cp -R * $DEBIAN_DIR/debian/debian-test/

for file in make-install make-package.sh README; 
do
    sudo cp ../debian-swig/$file $DEBIAN_DIR/debian/debian-swig/
done

sudo sed -i 's/amd64/i386/' $DEBIAN_DIR/debian/debian-test/debian/DEBIAN/control
