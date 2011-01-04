#!/bin/sh
rv=0
for f in *.cc *.h implementations/*.cc implementations/*.h ; do \
    if egrep -m 5 -n '.{81,}' $f ; then \
        echo "Overlong lines in $f!" ; \
        rv=1 ;\
    fi ; \
    if fgrep -m 5 -n '	' $f ; then \
        echo "Literal tabs in $f!" ; \
        rv=1 ; \
    fi ; \
done
exit $rv
