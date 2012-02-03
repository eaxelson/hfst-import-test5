#!/bin/sh
#
# Make a copy of the current directory without the .svn files
#

cd ..
if ! [ -d debian-test-copy ]; then
    mkdir debian-test-copy;
fi

if ! (tar -c --exclude=.svn debian-test | tar -x -C debian-test-copy); then
    exit 1;
fi

cd debian-test-copy
mv debian-test/* .
rmdir debian-test
