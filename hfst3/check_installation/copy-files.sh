#!/bin/sh

# Copy hfst3/test/tools/
cd ../test/tools
files_to_copy=`svn list`
cd ../../check_installation

for file in $files_to_copy;
do
    cp ../test/tools/$file .
done

# Make the transducers needed by the tests.
# Overgenerating some files here..
for att_file in *.txt;
do
    file=`echo $att_file | sed 's/\.txt//'`
    hfst-txt2fst -e '@0@' $file.txt > $file.hfst
    hfst-txt2fst -f sfst -e '@0@' $file.txt > $file.sfst
    hfst-txt2fst -f openfst-tropical -e '@0@' $file.txt > $file.ofst
    hfst-txt2fst -f foma -e '@0@' $file.txt > $file.foma
    hfst-txt2fst -e '@0@' -i $file.txt | hfst-fst2fst -w -o $file.hfstol
    hfst-txt2fst -e '@0@' -i $file.txt | hfst-invert | hfst-fst2fst -w -o $file.genhfstol
done

# Modify the tests so that they use the installed version of tools
# and refer to right files.
sed -i 's/$TOOLDIR\///' *.sh
sed -i 's/$srcdir\//.\//' *.sh
sed -i 's/hfst-proc\/hfst-apertium-proc/hfst-apertium-proc/' *.sh

# TODO: These tests are rewritten in directory check_installation
rm empty-input.sh
rm hfst-check-version.sh
rm hfst-check-help.sh

# These tests are excluded from test/tools/Makefile.am
rm incompatible-formats.sh
rm latin-1-strings.sh

# Perform the tests
for tooltest in *.sh;
do
    if [ "$tooltest" != "copy-files.sh" -a \
	"$tooltest" != "lexc2fst-stress.sh" -a \
	"$tooltest" != "valgrind.sh" -a \
	"$tooltest" != "lookup-stress.sh" ]; then
	echo -n "TEST: "$tooltest"...   "
	if ./$tooltest; then
	    echo "OK"
	else
	    echo "FAIL"
	fi
    fi
done
