#!/bin/sh
for i in "" .sfst .ofst .foma; do 
    if test -f cat.hfst$i -a -f cat2dog.hfst$i ; then
        if ! ../src/hfst-compose cat.hfst$i cat2dog.hfst$i > test.hfst ; then
            echo cat.hfst$1 o cat2dog.hfst$i fail
            exit 1
        fi
        if ! ../src/hfst-compare test.hfst cat2dog.hfst$i  ; then
            echo cat2doghfst$i differs
            exit 1
        fi
        rm test.hfst;
        if test -f identity-star.hfst$i ; then
            if ! ../src/hfst-compose cat.hfst$i identity-star.hfst$i > test.hfst ; then
                echo cat.hfst$1 o identitytstar.hfst$i fail
                exit 1
            fi
            if ! ../src/hfst-compare cat.hfst$i test.hfst > /dev/null ; then
                exit 1
            fi
        fi
        if test -f unknown-star.hfst$i ; then
            if ! ../src/hfst-compose cat.hfst$i identity-star.hfst$i > test.hfst ; then
                exit 1
            fi
        fi
	if test -f unknown2a.hfst$i && test -f identity.hfst$i ; then
	    if ! ../src/hfst-compose unknown2a.hfst$i identity.hfst$i > test.hfst ; then
		exit 1
            fi
	    if ../src/hfst-compare test.hfst unknown2a.hfst$i > /dev/null ; then
		exit
	    fi
	fi 
    fi
done