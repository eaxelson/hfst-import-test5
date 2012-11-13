#!/bin/sh

for pytest in \
    test_constructors.py test_flag_diacritics.py \
    test_hfst_basic_transducer.py test_streams.py test_tokenizer.py \
    test_transducer_functions.py test_extensions.py \
    test_examples.py test_xre_compiler.py ; #test_lexc.py;
    do
    if (! python $pytest); then
	echo "============================================="
	echo "FAIL: " $pytest;
	echo "============================================="
	exit 1;
    fi;
done

if ! ./test_std_streams.sh python; then 
    echo "==========================="
    echo "FAIL: test_std_streams.sh";
    echo "==========================="
    exit 1;
fi

echo "====================="
echo "All 10 tests passed."
echo "====================="