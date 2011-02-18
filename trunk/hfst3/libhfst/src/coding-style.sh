#!/bin/sh
rv=0
for f in *.cc implementations/*.cc ; do \
    if test "$f" != "implementations/xre_lex.cc" && \
    test "$f" != "implementations/xre_parse.cc" ; then \
    if egrep -m 5 -n '.{81,}' $f ; then \
        echo "Overlong lines in $f!" ; \
        rv=1 ;\
    fi ; \
    if fgrep -m 5 -n '	' $f ; then \
        echo "Literal tabs in $f!" ; \
        rv=1 ; \
    fi ; \
    fi ; \
done
for f in *.h implementations/*.h ; do \
    if fgrep -m 5 -n '	' $f ; then \
        echo "Literal tabs in $f!" ; \
        rv=1 ; \
    fi ; \
done
exit $rv
