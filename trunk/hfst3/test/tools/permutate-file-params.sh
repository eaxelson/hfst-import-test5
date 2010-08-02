#!/bin/sh
for f in ../../tools/src/hfst-{conjunct,disjunct,compose,subtract} ; do
    if [ -x "$f" ] ; then
        # well, not all permutations, but reasonable
        $f -1 cat.hfst -2 dog.hfst > test_named1named2stdout.hfst || exit 1
        $f cat.hfst dog.hfst > test_file1file2stdout.hfst || exit 1
        $f -1 cat.hfst dog.hfst > test_named1file2stdout.hfst || exit 1
        $f -2 dog.hfst cat.hfst > test_named2file1stdout.hfst || exit 1
        $f cat.hfst -2 dog.hfst > test_file1named2stdout.hfst || exit 1
        $f dog.hfst -1 cat.hfst > test_file2named1stdout.hfst || exit 1
        $f cat.hfst < dog.hfst > test_file1stdin2stdout.hfst || exit 1
        $f -1 cat.hfst < dog.hfst > test_named1stdin2stdout.hfst || exit 1
        $f -2 dog.hfst < cat.hfst > test_named2stdin1stdout.hfst || exit 1
        $f -1 cat.hfst -2 dog.hfst -o test_named1named2namedout.hfst || exit 1
        $f cat.hfst dog.hfst -o test_file1file2namedout.hfst || exit 1
        $f -1 cat.hfst dog.hfst -o test_named1file2namedout.hfst || exit 1
        $f -2 dog.hfst cat.hfst -o test_named2file1namedout.hfst || exit 1
        $f cat.hfst -2 dog.hfst -o test_file1named2namedout.hfst || exit 1
        $f dog.hfst -1 cat.hfst -o test_file2named1namedout.hfst || exit 1
        $f cat.hfst -o test_file1stdin2stdout.hfst  < dog.hfst || exit 1
        $f -1 cat.hfst -o test_named1stdin2namedout.hfst < dog.hfst  || exit 1
        $f -2 dog.hfst -o test_named2stdin1namedout.hfst < cat.hfst  || exit 1
        rm test_*.hfst
    fi
done

