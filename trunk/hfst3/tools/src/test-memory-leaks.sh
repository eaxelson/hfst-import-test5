#!/bin/sh

VALGRIND="valgrind --tool=memcheck --leak-check=full "

SHORTER_TEST_STRING="(a:bc:de:fg:hi:jk:lm:no:pq:rs:tu:vw:xy:z)| \
(A:BC:DE:FG:HI:JK:LM:NO:PQ:RS:TU:VW:XY:Z)"

LONGER_TEST_STRING="(a:bc:de:fg:hi:jk:lm:no:pq:rs:tu:vw:xy:z)| \
(A:BC:DE:FG:HI:JK:LM:NO:PQ:RS:TU:VW:XY:Z)| \
(A:bc:de:fg:hi:jk:lm:no:pq:rs:tu:vw:xy:Z)| \
(a:BC:DE:FG:HI:JK:LM:NO:PQ:RS:TU:VW:XY:z)"

for impl in sfst openfst-tropical foma;
do 
    echo $SHORTER_TEST_STRING | ./hfst-calculate -f $impl > tr.$impl;
    echo $LONGER_TEST_STRING | ./hfst-calculate -f $impl > TR.$impl;
done


if [ "$1" = "--all" ]; then
    echo "Performing an extensive check";
else
    echo "Testing program $1 for implementation type $2...";
    $VALGRIND .libs/$1 -i tr.$2 -o /dev/null 2> log;
    $VALGRIND .libs/$1 -i TR.$2 -o /dev/null 2> LOG;
    echo "Wrote log to files log and LOG."
    exit 0;
fi

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


for tool in $UNARY_TOOLS;
do
    for impl in sfst openfst-tropical foma;
    do
	echo -n "Testing program $tool for implementation type $impl: ";
	$VALGRIND .libs/$tool -i tr.$impl -o /dev/null 2> log;
	grep "definitely" log | grep -v "are" | tr '\n' ' ';
	$VALGRIND .libs/$tool -i TR.$impl -o /dev/null 2> LOG;
	grep "definitely" log | grep -v "are";
    done;
done

rm tr.sfst tr.openfst-tropical tr.foma
rm LOG log
