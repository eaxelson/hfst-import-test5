#!/bin/sh
#
# Make a copy of the current directory without the .svn files
#

cd ..
if ! [ -d debian-swig-copy ]; then
    mkdir debian-swig-copy;
fi

if ! (tar -c --exclude=.svn debian-swig | tar -x -C debian-swig-copy); then
    exit 1;
fi

cd debian-swig-copy
mv debian-swig/* .
rmdir debian-swig
