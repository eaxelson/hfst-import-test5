#!/bin/sh
for i in .sfst .ofst .foma; do
    if ! ../../tools/src/hfst-shuffle ab$i bc$i > TMP; then
	rm TMP;
	exit 1;
    else
	if ! ../../tools/src/hfst-compare TMP ab_shuffle_bc$i > /dev/null 2>&1; then
	    rm TMP;
	    exit 1;
	fi
    fi
    if ! ../../tools/src/hfst-shuffle id$i id$i > TMP; then
	rm TMP;
	exit 1;
    else
	if ! ../../tools/src/hfst-compare TMP id_shuffle_id$i > /dev/null 2>&1; then
	    rm TMP;
	    exit 1;
	fi
    fi
    if ! ../../tools/src/hfst-shuffle aid$i idb$i > TMP; then
	rm TMP;
	exit 1;
    else
	if ! ../../tools/src/hfst-compare TMP aid_shuffle_idb$i > /dev/null 2>&1; then
	    rm TMP;
	    exit 1;
	fi
    fi
    if ../../tools/src/hfst-shuffle a2b$i ab$i > /dev/null 2>&1; then
	rm TMP;
	exit 1;
    fi
    rm TMP;
done
