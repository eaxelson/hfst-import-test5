#!/bin/sh
for i in "" .sfst .ofst .foma; do
    if test -f cat_or_dog.hfst$i -a -f dog.hfst$i -a -f cat.hfst$i ; then
        if ! ../../tools/src/hfst-subtract cat_or_dog.hfst$i dog.hfst$i > test.hfst ; then
            exit 1
        fi
        if ! ../../tools/src/hfst-compare test.hfst cat.hfst$i  ; then
            exit 1
        fi
        rm test.hfst;
    else
	exit 1 ;
    fi
# test the empty transducer
    if test -f empty.hfst$i ; then
	if ! ../../tools/src/hfst-subtract empty.hfst$i empty.hfst$i > test.hfst ; then
	    exit 1
	fi
	if ! ../../tools/src/hfst-compare test.hfst empty.hfst$i ; then
	    exit 1
	fi
    else
        echo "FAIL: Missing files in empty transducer tests"
	exit 1 ;
    fi
# test that the complement [ [ID:ID | UNK:UNK]* - transducer ] works
    if test -f unk_or_id_star.hfst$i -a a2b.hfst$i -a a2b_complement.hfst$i -a a2b_input_projection_complement.hfst$i ; then
	if ! ../../tools/src/hfst-subtract -1 unk_or_id_star.hfst$i -2 a2b.hfst$i > test.hfst ; then
	    exit 1
	fi
	if ! ../../tools/src/hfst-compare -s test.hfst a2b_complement.hfst$i ; then
	    echo "FAIL: Complement test" $i
	    exit 1
	fi
	# the input projection
	if ! ../../tools/src/hfst-project -p input a2b.hfst$i > a2b_input.hfst ; then
	    exit 1
	fi
	if ! ../../tools/src/hfst-subtract -1 unk_or_id_star.hfst$i -2 a2b_input.hfst > test.hfst ; then
	    exit 1
	fi
	if ! ../../tools/src/hfst-compare -s test.hfst a2b_input_projection_complement.hfst$i ; then
	    echo "FAIL: Complement test, input projection" $i ;
	    exit 1
	fi
	rm a2b_input.hfst
	rm test.hfst;
    else
	echo "FAIL: Missing files in complement tests"
	exit 1 ;
    fi
done

# test weight handling for tropical transducers
if test -f cat2dog_0.3.hfst.ofst -a -f cat2dog_0.5.hfst.ofst ; then
    if ! ../../tools/src/hfst-subtract -1 cat2dog_0.3.hfst.ofst -2 cat2dog_0.5.hfst.ofst > test.hfst ; then
	exit 1;
    fi
    if ! ../../tools/src/hfst-compare empty.hfst.ofst test.hfst ; then
	exit 1;
    fi
    if ! ../../tools/src/hfst-subtract -2 cat2dog_0.3.hfst.ofst -1 cat2dog_0.5.hfst.ofst > test.hfst ; then
	exit 1;
    fi
    if ! ../../tools/src/hfst-compare empty.hfst.ofst test.hfst ; then
	exit 1;
    fi
    rm test.hfst;
else
    echo "FAIL: Missing files in weight tests"
    exit 1 ;
fi
