#!/bin/bash

# This script removes all non-directories in the current directory except
# those that are given as the arguments of the script.

if [ -d tempdir ]; then
    echo "ERROR: directory 'tempdir' already exists!";
    exit 1;
fi

mkdir tempdir;

for i in $@;
do
    mv $i tempdir/;
done


# list all non-directories, append 'rm' to the beginning and execute
# the remove commands
ls -F | egrep -v '/$' | sed 's/^\(.*\)$/rm "\1"/;' | bash


mv tempdir/* .;
rmdir tempdir;
