#!/bin/sh

VALGRIND="valgrind --tool=memcheck --leak-check=full "

for impl in sfst openfst-tropical foma;
do 
    echo "a:b" | ./hfst-calculate -f $impl > a2b.$impl;
done

UNARY_TOOLS="hfst-determinize \
hfst-fst2fst \
hfst-fst2strings \
hfst-fst2txt \
hfst-head \
hfst-invert \
hfst-minimize \
hfst-name \
hfst-project \
hfst-push-weights \
hfst-remove-epsilons \
hfst-repeat \
hfst-reverse \
hfst-reweight \
hfst-substitute \
hfst-summarize \
hfst-tail \
hfst-txt2fst"


for tool in $UNARY_TOOLS
do
    for impl in sfst openfst-tropical foma;
    do
	echo "Testing program $tool for implementation type $impl";
	$VALGRIND .libs/$tool -i a2b.$impl -o /dev/null 2> LOG;
	grep "definitely" LOG | grep -v "are";
    done;
done

rm a2b.sfst a2b.openfst-tropical a2b.foma
rm LOG
