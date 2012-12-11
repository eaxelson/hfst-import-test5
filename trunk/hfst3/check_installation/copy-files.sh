#!/bin/sh

# Copy hfst3/test/tools/
#cd ../test/tools
#files_to_copy=`svn list`
#cd ../../check_installation

#for file in $files_to_copy;
#do
#    cp ../test/tools/$file .
#done

# Make the transducers needed by the tests.
# Overgenerating some files here..
#for file in *.txt;
#do
#    hfst-txt2fst -e '@0@' $file > $file.hfst
#    cp $file.hfst `echo $file | sed 's/\.txt//'`".hfst"
#    hfst-txt2fst -f sfst -e '@0@' $file > $file.sfst
#    hfst-txt2fst -f openfst-tropical -e '@0@' $file > $file.ofst
#    hfst-txt2fst -f foma -e '@0@' $file > $file.foma
#    hfst-txt2fst -e '@0@' -i $file | hfst-fst2fst -w -o $file.hfstol
#    hfst-txt2fst -e '@0@' -i $file | hfst-invert | hfst-fst2fst -w -o $file.genhfstol
#done

# Modify the tests so that they use the installed version of tools
# and refer to right files.
#sed -i 's/$TOOLDIR\///' *.sh
#sed -i 's/$srcdir\//.\//' *.sh

# Perform the tests
for tooltest in *.sh;
do
    if [ "$tooltest" != "copy-files.sh" -a \
	"$tooltest" != "lexc2fst-stress.sh" -a \
	"$tooltest" != "valgrind.sh" ]; then
	echo -n "TEST: "$tooltest"...   "
	if ./$tooltest; then
	    echo "OK"
	else
	    echo "FAIL"
	fi
    fi
done
