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

done

exit 0;