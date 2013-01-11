#!/bin/sh
TOOLDIR=../../tools/src

EXT=
if (uname | egrep "MINGW|mingw" 2>1 > /dev/null); then
    EXT=".exe";
fi

verbose="true"
arg=$1
if [ "$arg" != "--verbose" ]; then
    verbose="false"
fi

# $TOOLDIR/../../scripts/hfst-xfst is excluded from tests as long as 
# it is just a wrapper script around foma that doesn't recognize 
# the switch '--help'.
# The tools hfst-train-tagger and hfst-twolc are scripts that call 
# other tools in a pipeline. That is why we use a local version of them
# when testing. They also print their help messages to standard error
# to avoid giving the message to the next tool in the pipeline.
#
for f in $TOOLDIR/hfst-* $TOOLDIR/hfst-proc/hfst-* \
    $TOOLDIR/hfst-tagger/src/hfst-tag $TOOLDIR/hfst-tagger/src/hfst-train-tagger-loc \
    $TOOLDIR/hfst-twolc/src/hfst-twolc-loc ; do
    # Skip deprecated tools that can still lay around somewhere
    if [ "$f" != "$TOOLDIR/hfst-xfst2fst""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-lexc-compiler""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-twolc-system""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-duplicate""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-preprocess-for-optimized-lookup-format""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-strip-header""$EXT" -a \
	"$f" != "$TOOLDIR/hfst-train-tagger-system""$EXT" ]; then
        if [ -x "$f" -a ! -d "$f" ] ; then
	    if [ "$f" != "$TOOLDIR/hfst-tagger/src/hfst-train-tagger-loc" -a \
		"$f" != "$TOOLDIR/hfst-twolc/src/hfst-twolc-loc" ] ; then
		if ! "$f" --help > help.out ; then
                    rm help.out
                    echo $f has broken help
                    exit 1
		fi
		if ! grep -q '^Usage: hfst-' help.out > /dev/null ; then
                    rm help.out
                    echo $f has malformed help
                    exit 1
		fi
            else
		if ! "$f" --help 2> help.out ; then
                    rm help.out
                    echo $f has broken help
                    exit 1
		fi
		if ! grep -q '^Usage: hfst-' help.out > /dev/null ; then
                    rm help.out
                    echo $f has malformed help
                    exit 1
		fi
	    fi
	    if [ "$verbose" != "false" ]; then
		echo "PASS: "$f
	    fi
	fi
    else
	if [ "$verbose" != "false" ]; then
	    echo "Skipping "$f"..."
	fi
    fi
    
done

rm help.out

