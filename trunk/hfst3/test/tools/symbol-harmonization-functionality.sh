#!/bin/sh

TOOLDIR=../../tools/src/

for ext in .sfst .ofst .foma; do

    FFLAG=
    case $ext in
        .sfst)
            FFLAG="-f sfst"
            if ! ($TOOLDIR/hfst-format --list-formats | grep "sfst" > /dev/null) ; then
                continue;
            fi;;
        .ofst)
            FFLAG="-f openfst-tropical"
            if ! ($TOOLDIR/hfst-format --list-formats | grep "openfst-tropical" > /dev/null) ; then
                continue;
            fi;;
        .foma)
            FFLAG="-f foma"
            if ! ($TOOLDIR/hfst-format --list-formats | grep "foma" > /dev/null) ; then
                continue;
            fi;;
        *)
            FFLAG=;;
    esac

    # composition

    # harmonization:  [a:b] .o. [ID:ID] == [a:b]
    if ! ($TOOLDIR/hfst-compose a2b$ext id$ext | $TOOLDIR/hfst-compare -s a2b$ext); then
	echo "#1" ${FFLAG}
	exit 1;
    fi
    # harmonization:  [ID:ID] .o. [a:b] == [a:b]
    if ! ($TOOLDIR/hfst-compose id$ext a2b$ext | $TOOLDIR/hfst-compare -s a2b$ext); then
	echo "#2" ${FFLAG}
	exit 1;
    fi

    # harmonization:  [a:b] .o. [UNK:UNK | ID:ID] == [[a:a] | [a:b] | [a:UNK]]
    if ! ($TOOLDIR/hfst-compose a2b$ext unk2unk_or_id$ext | $TOOLDIR/hfst-compare -s a2a_or_a2b_or_a2unk$ext); then
	echo "#3" ${FFLAG}
	exit 1;
    fi
    # harmonization:  [UNK:UNK | ID:ID] .o. [a:b] == [[a:b] | [b:b] | [UNK:b]]
    if ! ($TOOLDIR/hfst-compose unk2unk_or_id$ext a2b$ext | $TOOLDIR/hfst-compare -s a2b_or_b2b_or_unk2b$ext); then
	echo "#4" ${FFLAG}
	exit 1;
    fi


    # no harmonization:    [a:b] .o. [ID:ID] == empty
    if ! ($TOOLDIR/hfst-compose -H a2b$ext id$ext | $TOOLDIR/hfst-compare -s empty$ext); then
	echo "#5" ${FFLAG}
	exit 1;
    fi
    # no harmonization:    [ID:ID] .o. [a:b] == empty
    if ! ($TOOLDIR/hfst-compose -H id$ext a2b$ext | $TOOLDIR/hfst-compare -s empty$ext); then
	echo "#6" ${FFLAG}
	exit 1;
    fi

    # no harmonization:    [a:b] .o. [UNK:UNK | ID:ID] == empty
    if ! ($TOOLDIR/hfst-compose -H a2b$ext unk2unk_or_id$ext | $TOOLDIR/hfst-compare -s empty$ext); then
	echo "#7" ${FFLAG}
	exit 1;
    fi
    # no harmonization:    [UNK:UNK | ID:ID] .o. [a:b] == empty
    if ! ($TOOLDIR/hfst-compose -H unk2unk_or_id$ext a2b$ext | $TOOLDIR/hfst-compare -s empty$ext); then
	echo "#8" ${FFLAG}
	exit 1;
    fi


    continue;

    # concatenation
    echo "[?]" | $TOOLDIR/hfst-regexp2fst ${FFLAG} > tmp1;
    echo "[a:b]" | $TOOLDIR/hfst-regexp2fst ${FFLAG} > tmp2;

    # harmonization
    $TOOLDIR/hfst-concatenate tmp1 tmp2 | $TOOLDIR/hfst-minimize > concatenation;
    echo "[[?|a|b] a:b]" | $TOOLDIR/hfst-regexp2fst ${FFLAG} | $TOOLDIR/hfst-minimize > result;
    # do not harmonize when comparing, the transducers must be exactly the same
    if ! ($TOOLDIR/hfst-compare -H -s concatenation result); then
	echo "#9" ${FFLAG}
	exit 1;
    fi

    # no harmonization
    $TOOLDIR/hfst-concatenate -H tmp1 tmp2 | $TOOLDIR/hfst-minimize > concatenation;
    echo "[?|a:b]" | $TOOLDIR/hfst-regexp2fst ${FFLAG} | $TOOLDIR/hfst-minimize > result;
    # do not harmonize when comparing, the transducers must be exactly the same
    if ! ($TOOLDIR/hfst-compare -H -s concatenation result); then
	echo "#9" ${FFLAG}
	exit 1;
    fi


    # disjunction
    echo "[?]" | $TOOLDIR/hfst-regexp2fst ${FFLAG} > tmp1;
    echo "[a:b]" | $TOOLDIR/hfst-regexp2fst ${FFLAG} > tmp2;
    $TOOLDIR/hfst-disjunct tmp1 tmp2 | $TOOLDIR/hfst-minimize > disjunction;
    echo "[[?|a|b] a:b]" | $TOOLDIR/hfst-regexp2fst ${FFLAG} | $TOOLDIR/hfst-minimize > result;
    # do not harmonize when comparing, the transducers must be exactly the same
    if ! ($TOOLDIR/hfst-compare -H -s disjunction result); then
	echo "#10" ${FFLAG}
	exit 1;
    fi

done

exit 0;