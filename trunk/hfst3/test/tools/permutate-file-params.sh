#!/bin/sh
if [ -x ../../tools/src/hfst-compare ] ; then
    # well, not all permutations, but reasonable
    if hfst-compare -1 cat.hfst -2 dog.hfst ; then
       exit 1
    fi
    if hfst-compare -1 cat.hfst dog.hfst ; then
       exit 1
    fi
    if hfst-compare -2 dog.hfst cat.hfst ; then
       exit 1
    fi
    if hfst-compare dog.hfst -1 cat.hfst ; then
       exit 1
    fi
    if hfst-compare cat.hfst -2 dog.hfst ; then
       exit 1
    fi
    if hfst-compare cat.hfst < dog.hfst ; then
        exit 1
    fi
    if hfst-compare -1 cat.hfst < dog.hfst ; then
        exit 1
    fi
    if hfst-compare -2 dog.hfst < cat.hfst ; then
        exit 1
    fi
fi
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
        for g in test_*.hfst ; do
            for h in test_*.hfst ; do
                if ! hfst-compare $g $h ; then
                    echo "$f builds $g and $h differently from same sources"
                    exit 1
                fi
            done
        done
        rm test_*.hfst
    fi
done

for f in ../../tools/src/hfst-{determinize,invert,minimize,remove-epsilons,reverse} ; do
    if [ -x $f ] ; then
        $f -i cat.hfst > test_namedinstdout.hfst || exit 1
        $f cat.hfst > test_fileinstdout.hfst || exit 1
        $f -i cat.hfst -o test_namedinnamedout.hfst || exit 1
        $f cat.hfst -o test_fileinnamedout.hfst || exit 1
        for g in test_*.hfst ; do
            for h in test_*.hfst ; do
                if ! hfst-compare $g $h ; then
                    echo "$f builds $g and $h differently from same sources"
                    exit 1
                fi
            done
        done
        rm test_*.hfst
    fi
done

for f in ../../tools/src/hfst-{fst2strings,fst2txt} ; do
    if [ -x $f ] ; then
        $f -i cat.hfst > test_namedinstdout.txt || exit 1
        $f cat.hfst > test_fileinstdout.txt || exit 1
        $f -i cat.hfst -o test_namedinnamedout.txt || exit 1
        $f cat.hfst -o test_fileinnamedout.txt || exit 1
        for g in test_*.txt ; do
            for h in test_*.txt ; do
                if diff $g $h ; then
                    echo "$f builds $g and $h differently from same sources"
                    exit 1
                fi
            done
        done
        rm test_*.txt
    fi
done


