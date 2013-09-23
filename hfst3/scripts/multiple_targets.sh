#!/bin/sh

files=`find . -name '*.Plo'`
for file in $files;
do
    echo $file;
    # sed -i 's/c\:\\/\/cygdrive\/c\//' $file;
done
