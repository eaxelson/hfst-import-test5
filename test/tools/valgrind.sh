#!/bin/sh
HFST_FLAGS="--quiet --output=/dev/null"
VALGRIND="libtool --mode=execute valgrind --leak-check=full"
if ! test -d valgrind-logs ; then
    mkdir valgrind-logs/
fi
if test -z "$srcdir" ; then
    srcdir=./
fi
p=calculate
for i in "" .sfst .ofst .foma; do
    FFLAG=
    case $i in
        .sfst)
            FFLAG="-f sfst";;
        .ofst)
            FFLAG="-f openfst-tropical";;
        .foma)
            FFLAG="-f foma";;
        *)
            FFLAG=;;
    esac
    echo "catcatcat(cat)+" | $VALGRIND --log-file=valgrind-logs/${p}4toINFcats.hfst$i.log ../../tools/src/hfst-calculate $FFLAG $HFST_FLAGS
    echo "{cat}:{dog}" | $VALGRIND --log-file=valgrind-logs/${p}cat2dog.hfst$i.log ../../tools/src/hfst-calculate $FFLAG $HFST_FLAGS
done
p=compare
comparables="cat.hfst dog.hfst tac.hfst cat2dog.hfst dog2cat.hfst cat_or_dog.hfst catdog.hfst"
for f in $comparables; do
    $VALGRIND --log-file=valgrind-logs/${p}${f}${f}.log ../../tools/src/hfst-compare $f $f $HFST_FLAGS
    for g in $comparables ; do
        if test $f != $g ; then
            $VALGRIND --log-file=valgrind-logs/${p}${f}${g} hfst-compare $f $g
        fi
    done
done

p=compose
for i in "" .sfst .ofst .foma; do 
    if test -f cat.hfst$i -a -f cat2dog.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}catcat2dog.hfst$i.log ../../tools/src/hfst-compose cat.hfst$i cat2dog.hfst$i
        if test -f identity-star.hfst$i ; then
            $VALGRIND --log-file=valgrind-logs/${p}catidentity-star.hfst$i.log ../../tools/src/hfst-compose cat.hfst$i identity-star.hfst$i
        fi
        if test -f unknown-star.hfst$i ; then
            $VALGRIND --log-file=valgrind-logs/${p}catunknown-star.hfst${i}.log  ../../tools/src/hfst-compose cat.hfst$i identity-star.hfst$i 
        fi
    
        if test -f unknown2a.hfst$i && test -f identity.hfst$i ; then
            $VALGRIND --log-file=valgrind-logs/${p}unknown2aideentity.hfst${i}.log  ../../tools/src/hfst-compose unknown2a.hfst$i identity.hfst$i 
        fi
    fi
done
p=compose-intersect
for i in "" .sfst .ofst .foma; do
    FFLAG=
    case $i in
        .sfst)
            FFLAG="-f sfst";;
        .ofst)
            FFLAG="-f openfst-tropical";;
        .foma)
            FFLAG="-f foma";;
        *)
            FFLAG=;;
    esac
    if test -f cat.hfst$i ; then
        echo "cat|dog" | ../../tools/src/hfst-calculate $FFLAG > lexicon.hfst
        echo "([catdog]|d:D|c:C)*" | ../../tools/src/hfst-calculate $FFLAG > rules.hfst
        echo "([catdog]|d:D|c:C)*" | ../../tools/src/hfst-calculate $FFLAG >> rules.hfst
        $VALGRIND --log-file=valgrind-logs/${p}catordoguppercase.hfst${i}.log  ../../tools/src/hfst-compose-intersect -1 lexicon.hfst -2 rules.hfst 
        rm lexicon.hfst rules.hfst
    fi
done
p=concatenate
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i -a -f dog.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}catdog.hfst${i}.log  ../../tools/src/hfst-concatenate cat.hfst$i dog.hfst$i 
    fi
done
p=conjunct
for i in "" .sfst .ofst .foma; do
    if test -f cat_or_dog.hfst$i -a -f cat.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}catdog.hfst${i}.log  ../../tools/src/hfst-conjunct cat_or_dog.hfst$i cat.hfst$i 
    fi
done
p=determinize
for i in "" .sfst .ofst .foma; do
    if test -f non_minimal.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}nonminimal.hfst${i}.log  ../../tools/src/hfst-determinize non_minimal.hfst$i 
    fi
done
p=disjunct
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i -a -f dog.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}catdog.hfst${i}.log  ../../tools/src/hfst-disjunct cat.hfst$i dog.hfst$i 
    fi
done
p=fst2strings
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}cat.hfst${i}.log  ../../tools/src/hfst-fst2strings cat.hfst$i
    fi
done
p=fst2txt
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}cat.hfst${i}.log  ../../tools/src/hfst-fst2txt -D < cat.hfst$i
    fi
done
p=head
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a -f dog2cat.hfst$i ; then
        cat cat2dog.hfst$i dog2cat.hfst$i | $VALGRIND --log-file=valgrind-logs/${p}2cat2dog.hfst${i}.log ../../tools/src/hfst-head -n 1 
    fi
done
p=invert
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}cat2dog.hfst${i}.log  ../../tools/src/hfst-invert cat2dog.hfst$i 
    fi
done
p=lookup
$VALGRIND --log-file=valgrind-logs/${p}cat.hfst${i}.log  ../../tools/src/hfst-lookup cat.hfst < $srcdir/cat.strings

# test what strings the transducer [a:b (ID:ID)*] recognizes
for i in "" .sfst .ofst .foma; do

    echo "aa" | $VALGRIND --log-file=valgrind-logs/${p}abid.hfst${i}.log  ../../tools/src/hfst-lookup abid.hfst$i
    echo "ac" | $VALGRIND --log-file=valgrind-logs/${p}name.hfst${i}.log  ../../tools/src/hfst-lookup abid.hfst$i 
done

p=minimize
for i in "" .sfst .ofst .foma; do
    if test -f non_minimal.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}nonminimal.hfst${i}.log  ../../tools/src/hfst-minimize non_minimal.hfst$i 
    fi
done
p=proc
echo "cat" | $VALGRIND --log-file=valgrind-logs/${p}cat.hfst${i}.log  ../../tools/src/hfst-proc/hfst-proc cat2dog.hfst.ol
echo "^dog$" | $VALGRIND --log-file=valgrind-logs/${p}gen.hfst${i}.log  ../../tools/src/hfst-proc/hfst-proc -g cat2dog.gen.hfst.ol
echo "cat" | $VALGRIND --log-file=valgrind-logs/${p}weight.hfst${i}.log  ../../tools/src/hfst-proc/hfst-proc -W cat_weight_final.hfst.ol 
$VALGRIND --log-file=valgrind-logs/${p}caps.hfst${i}.log  ../../tools/src/hfst-proc/hfst-proc proc-caps.hfst.ol < $srcdir/proc-caps-in.strings
$VALGRIND --log-file=valgrind-logs/${p}capsgen.hfst${i}.log  hfst-proc -g proc-caps.gen.hfst.ol < $srcdir/proc-caps-gen.strings
$VALGRIND --log-file=valgrind-logs/${p}capsc.hfst${i}.log  ../../tools/src/hfst-proc/hfst-proc -c proc-caps.hfst.ol < $srcdir/proc-caps-in.strings
$VALGRIND --log-file=valgrind-logs/${p}capsw.hfst${i}.log  ../../tools/src/hfst-proc/hfst-proc -w proc-caps.hfst.ol < $srcdir/proc-caps-in.strings
$VALGRIND --log-file=valgrind-logs/${p}rawcg.hfst${i}.log  ../../tools/src/hfst-proc/hfst-proc --cg --raw proc-caps.hfst.ol < $srcdir/proc-caps-in.strings
p=project
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a cat.hfst$i -a dog.hfst$i ; then
        for j in input output; do
            $VALGRIND --log-file=valgrind-logs/${p}cat2dog${j}.hfst${i}.log  ../../tools/src/hfst-project -p $j cat2dog.hfst$i 
            ../../tools/src/hfst-concatenate cat2dog.hfst$i unk2unk.hfst$i > concatenation.hfst
            $VALGRIND --log-file=valgrind-logs/${p}unkcat2dog.hfst${i}.log  ../../tools/src/hfst-project -p $j concatenation.hfst
            rm concatenation.hfst
        done
    fi
done
p=push-weights
$VALGRIND --log-file=valgrind-logs/${p}cat2doginitial.hfst${i}.log  ../../tools/src/hfst-push-weights -p initial cat2dog.hfst 
$VALGRIND --log-file=valgrind-logs/${p}cat2dogfinal.hfst${i}.log  ../../tools/src/hfst-push-weights -p final cat2dog.hfst 
p=regexp2fst
for i in sfst openfst-tropical foma; do
    $VALGRIND --log-file=valgrind-logs/${p}catanddog.hfst${i}.log  ../../tools/src/hfst-regexp2fst -f $i $srcdir/cats_and_dogs.xre 
    $VALGRIND --log-file=valgrind-logs/${p}catanddogsemicolon.hfst${i}.log  ../../tools/src/hfst-regexp2fst -S -f $i $srcdir/cats_and_dogs_semicolon.xre 
done
p=remove-epsilons
for i in "" .sfst .ofst .foma; do
    if test -f non_minimal.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}nonminimal.hfst${i}.log  ../../tools/src/hfst-remove-epsilons non_minimal.hfst$i 
    fi
done
p=repeat
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i -a -f 2to4cats.hfst$i -a -f 0to3cats.hfst$i \
        -a -f 4cats.hfst$i -a -f 4toINFcats.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}2to4cats.hfst${i}.log  ../../tools/src/hfst-repeat -f 2 -t 4 cat.hfst$i 
        $VALGRIND --log-file=valgrind-logs/${p}0to3cats.hfst${i}.log  ../../tools/src/hfst-repeat -t 3 cat.hfst$i 
        $VALGRIND --log-file=valgrind-logs/${p}4cats.hfst${i}.log  ../../tools/src/hfst-repeat -f 4 -t 4 cat.hfst$i 
        $VALGRIND --log-file=valgrind-logs/${p}4toinfcats.hfst${i}.log  ../../tools/src/hfst-repeat -f 4 cat.hfst$i 
    fi
done
p=reverse-functionality.sh
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a dog2cat.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}cat2dog.hfst${i}.log  ../../tools/src/hfst-invert cat2dog.hfst$i 
    fi
done
p=split
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a -f dog2cat.hfst$i ; then
        cat cat2dog.hfst$i dog2cat.hfst$i | $VALGRIND --log-file=valgrind-logs/${p}cat2dog2cat.hfst${i}.log  ../../tools/src/hfst-split
        rm 1.hfst 2.hfst;
    fi
done
p=strings2fst
for i in "" .sfst .ofst .foma; do
    FFLAG=
    case $i in
        .sfst)
            FFLAG="-f sfst";;
        .ofst)
            FFLAG="-f openfst-tropical";;
        .foma)
            FFLAG="-f foma";;
        *)
            FFLAG=;;
    esac
    $VALGRIND --log-file=valgrind-logs/${p}cat.hfst${i}.log  ../../tools/src/hfst-strings2fst $FFLAG $srcdir/cat.strings 
    $VALGRIND --log-file=valgrind-logs/${p}c_a_t.hfst${i}.log  ../../tools/src/hfst-strings2fst $FFLAG -S $srcdir/c_a_t.strings 
    $VALGRIND --log-file=valgrind-logs/${p}heavycat.hfst${i}.log  ../../tools/src/hfst-strings2fst $FFLAG $srcdir/heavycat.strings 
    $VALGRIND --log-file=valgrind-logs/${p}cat2dog.hfst${i}.log  ../../tools/src/hfst-strings2fst $FFLAG $srcdir/cat2dog.strings 
    $VALGRIND --log-file=valgrind-logs/${p}c_a_t2d_o_g.hfst${i}.log  ../../tools/src/hfst-strings2fst $FFLAG -S $srcdir/cat2dog.spaces 
    $VALGRIND --log-file=valgrind-logs/${p}cat2dogpairs.hfst${i}.log  ../../tools/src/hfst-strings2fst $FFLAG -p -S $srcdir/cat2dog.pairs 
    $VALGRIND --log-file=valgrind-logs/${p}cat2dogps.hfst${i}.log  ../../tools/src/hfst-strings2fst $FFLAG -p $srcdir/cat2dog.pairstring 
    $VALGRIND --log-file=valgrind-logs/${p}name.hfst${i}.log  ../../tools/src/hfst-strings2fst $srcdir/utf-8.strings 
done

p=substitute
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i -a -f dog.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}cat2dog.hfst${i}.log  ../../tools/src/hfst-substitute cat.hfst$i -F $srcdir/cat2dog.substitute 
        $VALGRIND --log-file=valgrind-logs/${p}c2d.hfst${i}.log ../../tools/src/hfst-substitute cat.hfst$i -f c -t d 
    fi
done
p=subtract
for i in "" .sfst .ofst .foma; do
    if test -f cat_or_dog.hfst$i -a -f dog.hfst$i -a cat.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}sutractcatdog.hfst${i}.log  ../../tools/src/hfst-subtract cat_or_dog.hfst$i dog.hfst$i
    fi 
    if test -f empty.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}empty.hfst${i}.log  ../../tools/src/hfst-subtract empty.hfst$i empty.hfst$i 
    fi
    if test -f unk_or_id_star.hfst$i -a -f a2b.hfst$i -a -f a2b_complement.hfst$i -a -f a2b_input_projection_complement.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}unkida2b.hfst${i}.log  ../../tools/src/hfst-subtract -1 unk_or_id_star.hfst$i -2 a2b.hfst$i 
        ../../tools/src/hfst-project -p input a2b.hfst$i > a2b_input.hfst
        $VALGRIND --log-file=valgrind-logs/${p}unkida2binput.hfst${i}.log  ../../tools/src/hfst-subtract -1 unk_or_id_star.hfst$i -2 a2b_input.hfst 
        rm a2b_input.hfst
    fi
done
if test -f cat2dog_0.3.hfst.ofst -a -f cat2dog_0.5.hfst.ofst ; then
    $VALGRIND --log-file=valgrind-logs/${p}cat2dog0305.hfst${i}.log  ../../tools/src/hfst-subtract -1 cat2dog_0.3.hfst.ofst -2 cat2dog_0.5.hfst.ofst 
    $VALGRIND --log-file=valgrind-logs/${p}cat2dog0503.hfst${i}.log  ../../tools/src/hfst-subtract -2 cat2dog_0.3.hfst.ofst -1 cat2dog_0.5.hfst.ofst 
fi
p=summarize
for i in "" .sfst .ofst .foma; do
    if test -f cat.hfst$i ; then
        $VALGRIND --log-file=valgrind-logs/${p}cat.hfst${i}.log  ../../tools/src/hfst-summarize cat.hfst$i
    fi
done
p=tail
for i in "" .sfst .ofst .foma; do
    if test -f cat2dog.hfst$i -a -dog2cat.hfst$i ; then
        cat cat2dog.hfst$i dog2cat.hfst$i | $VALGRIND --log-file=valgrind-logs/${p}name.hfst${i}.log  ../../tools/src/hfst-tail -n 1 
    fi
done
p=txt2fst
for i in "" .sfst .ofst .foma; do
    FFLAG=
    case $i in
        .sfst)
            FFLAG="-f sfst";;
        .ofst)
            FFLAG="-f openfst-tropical";;
        .foma)
            FFLAG="-f foma";;
        *)
            FFLAG=;;
    esac
    $VALGRIND --log-file=valgrind-logs/${p}cat.hfst${i}.log  ../../tools/src/hfst-txt2fst $FFLAG $srcdir/cat.txt 
done
# valgrind.sh
