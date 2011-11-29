#!/bin/sh
#
# Make a copy of the current directory without the .svn files
#
cd ..
tar -c --exclude=.svn debian-test | tar -x -C debian-test-copy
cd debian-test-copy
mv debian-test/* .
rmdir debian-test
