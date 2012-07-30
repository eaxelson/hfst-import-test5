#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i ; then
        if ! ../src/hfst-edit-metadata -p cat.hfst$i > test.name ; then
            echo getting name prop failed
            exit 1
        fi
        if ! ../src/hfst-edit-metadata -a name=kissa cat.hfst$i > test.hfst ; then
            echo setting name prop failed
            exit 1
        fi
        if ! ../src/hfst-edit-metadata -p  test.hfst | fgrep kissa ; then
            echo regetting name prop failed
            exit 1
        fi
        if ! ../src/hfst-edit-metadata -a name=`head -c 256 long.strings` cat.hfst$i > test.hfst ; then
            echo failed setting name of length 256
            exit 1
        fi
        if ! ../src/hfst-edit-metadata -p test.hfst > test.name; then
            echo failed printing name of length 256
            exit 1
        fi
        if ! ../src/hfst-edit-metadata -a name=`cat long.strings` cat.hfst$i > test.hfst ; then
            echo failed setting name of length 65537
            exit 1
        fi
        if ! ../src/hfst-edit-metadata -p test.hfst > test.name; then
            echo failed printing name of length 65537
            exit 1
        fi
        rm test.hfst test.name;
    fi
done

