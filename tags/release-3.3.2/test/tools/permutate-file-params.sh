#!/bin/sh
if [ -x ../../tools/src/hfst-compare ] ; then
    # well, not all permutations, but reasonable
    if ../../tools/src/hfst-compare -s -1 cat.hfst -2 dog.hfst  ; then
       exit 1
    fi
    if ../../tools/src/hfst-compare -s -1 cat.hfst dog.hfst  ; then
       exit 1
    fi
    if ../../tools/src/hfst-compare -s -2 dog.hfst cat.hfst  ; then
       exit 1
    fi
    if ../../tools/src/hfst-compare -s dog.hfst -1 cat.hfst  ; then
       exit 1
    fi
    if ../../tools/src/hfst-compare -s cat.hfst -2 dog.hfst  ; then
       exit 1
    fi
    if ../../tools/src/hfst-compare -s cat.hfst < dog.hfst  ; then
        exit 1
    fi
    if ../../tools/src/hfst-compare -s -1 cat.hfst < dog.hfst  ; then
        exit 1
    fi
    if ../../tools/src/hfst-compare -s -2 dog.hfst < cat.hfst  ; then
        exit 1
    fi
fi
rm -f test_*.hfst
for f in ../../tools/src/hfst-{conjunct,disjunct,compose,subtract,compose,compose-intersect} ; do
    if [ -x "$f" ] ; then
        # well, not all permutations, but reasonable
        $f -1 cat.hfst -2 dog.hfst > test_named1named2stdout.hfst || exit 1
        $f cat.hfst dog.hfst > test_file1file2stdout.hfst || exit 1
        $f -1 cat.hfst dog.hfst > test_named1file2stdout.hfst || exit 1
        $f -2 dog.hfst cat.hfst > test_named2file1stdout.hfst || exit 1
        $f cat.hfst -2 dog.hfst > test_file1named2stdout.hfst || exit 1
        $f dog.hfst -1 cat.hfst > test_file2named1stdout.hfst || exit 1
        $f dog.hfst < cat.hfst > test_file1stdin2stdout.hfst || exit 1
        $f -1 cat.hfst < dog.hfst > test_named1stdin2stdout.hfst || exit 1
        $f -2 dog.hfst < cat.hfst > test_named2stdin1stdout.hfst || exit 1
        $f -1 cat.hfst -2 dog.hfst -o test_named1named2namedout.hfst || exit 1
        $f cat.hfst dog.hfst -o test_file1file2namedout.hfst || exit 1
        $f -1 cat.hfst dog.hfst -o test_named1file2namedout.hfst || exit 1
        $f -2 dog.hfst cat.hfst -o test_named2file1namedout.hfst || exit 1
        $f cat.hfst -2 dog.hfst -o test_file1named2namedout.hfst || exit 1
        $f dog.hfst -1 cat.hfst -o test_file2named1namedout.hfst || exit 1
        $f dog.hfst -o test_file1stdin2stdout.hfst  < cat.hfst || exit 1
        $f -1 cat.hfst -o test_named1stdin2namedout.hfst < dog.hfst  || exit 1
        $f -2 dog.hfst -o test_named2stdin1namedout.hfst < cat.hfst  || exit 1
        for g in test_*.hfst ; do
            for h in test_*.hfst ; do
                if ! ../../tools/src/hfst-compare -s $g $h  ; then
                    echo "$f builds $g and $h differently from same sources"
                    exit 1
                fi
            done
        done
        rm -f test_*.hfst
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
                if ! ../../tools/src/hfst-compare -s $g $h  ; then
                    echo "$f builds $g and $h differently from same sources"
                    exit 1
                fi
            done
        done
        rm -f test_*.hfst
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
                if ! cmp $g $h ; then
                    echo "$f builds $g and $h differently from same sources"
                    exit 1
                fi
            done
        done
        rm -f test_*.txt
    fi
done


