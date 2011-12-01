#!/bin/sh
#
#  A script for creating the debian package for HFST
#

#hfst_tool_dir=../../hfst3/tools/src/.libs/ # the HFST commandline tools
#hfst_library_dir=../../hfst3/libhfst/src/.libs/ # the HFST library
#hfst_header_dir=../../hfst3/libhfst/src/ # the HFST header files
#impl_library_dir=/usr/local/lib/  # the backend implementation libraries


#hfst_tools=hfst-invert # ...

# Copy the tools
#for tool in $hfst_tools;
#do
#    cp $hfst_tool_dir/$tool debian/usr/bin/;
#done
strip debian/usr/bin/*

# Copy the libraries
#cp -P $hfst_library_dir/libhfst.so* debian/usr/lib/
#cp -P $impl_library_dir/libfst.so* debian/usr/lib/

cd debian/usr/lib/

cp /usr/local/lib/libfst.so.0.0.0 .
rm -f libfst.so libfst.so.0
ln -s -T libfst.so.0.0.0 libfst.so.0
ln -s -T libfst.so.0 libfst.so

cp /usr/local/lib/libsfst.so.0.0.0 .
rm -f libsfst.so libsfst.so.0
ln -s -T libsfst.so.0.0.0 libsfst.so.0
ln -s -T libsfst.so.0 libsfst.so

#cp /usr/local/lib/libsfst.so.1.3 .
#rm -f libsfst.so.1 libsfst.so
#ln -s -T libsfst.so.1.3 libsfst.so.1
#ln -s -T libsfst.so.1 libsfst.so

cp /usr/local/lib/libfoma.so.0.9.14 .
rm .f libfoma.so.0 libfoma.so
ln -s -T libfoma.so.0.9.14 libfoma.so.0
ln -s -T libfoma.so.0 libfoma.so

strip *
chmod 0644 *

rm libhfst.la

cd ../../..

header_files="HfstTransducer.h hfst_apply_schemas.h HfstDataTypes.h \
    HfstSymbolDefs.h HarmonizeUnknownAndIdentitySymbols.h HfstExceptionDefs.h \
    HfstExtractStrings.h HfstFlagDiacritics.h HfstInputStream.h \
    HfstOutputStream.h HfstTokenizer.h \
    implementations/ConvertTransducerFormat.h \
    implementations/HfstTransitionGraph.h implementations/HfstTransition.h \
    implementations/HfstTropicalTransducerTransitionData.h \
    implementations/HfstFastTransitionData.h"

header_files=$header_files" implementations/SfstTransducer.h"

header_files=$header_files" implementations/FomaTransducer.h"

header_files=$header_files" implementations/TropicalWeightTransducer.h \
                            implementations/LogWeightTransducer.h"

header_files=$header_files" parsers/LexcCompiler.h"

header_files=$header_files" parsers/XreCompiler.h"

header_files=$header_files" implementations/HfstOlTransducer.h"

header_files=$header_files" \
    implementations/compose_intersect/ComposeIntersectRulePair.h \
    implementations/compose_intersect/ComposeIntersectLexicon.h \
    implementations/compose_intersect/ComposeIntersectRule.h \
    implementations/compose_intersect/ComposeIntersectFst.h"


# Copy the HFST header files
#for header in $header_files;
#do
#    cp -P $hfst_header_dir/$header debian/usr/include/;
#done
chmod 0644 debian/usr/include/*
#mkdir debian/usr/include/hfst
chmod 0755 debian/usr/include/hfst

# Configure the libraries
cp debian/DEBIAN/control debian/control
dpkg-shlibdeps debian/usr/bin/*
rm debian/control

# gzip the changelog file, if needed
if [ -e debian/usr/share/doc/hfst-dev/changelog.Debian ]; then
    gzip --force --best debian/usr/share/doc/hfst-dev/changelog.Debian;
fi

# move directory share under usr
#if [ -d debian/share ]; then
#    if ! [ -d debian/usr/share ]; then
#	mkdir debian/usr/share;
#    fi
#    cp -R debian/share/* debian/usr/share/;
#fi

# move directory include under usr
#if [ -d debian/include ]; then
#    if ! [ -d debian/usr/share ]; then
#	mkdir debian/usr/share;
#    fi
#    cp -R debian/share/* debian/usr/share/;
#fi

# gzip the man pages
gzip --best --force debian/usr/share/man/man1/*.1

# To make a copy without the .svn files
# tar -c --exclude=.svn debian-test | tar -x -C debian-test-copy
# cd debian-test-copy
# mv debian-test/* .
# rmdir debian-test

# Make the package
fakeroot dpkg-deb --build debian

# Test if the package is ok (lintian will probably give some warnings, 
# see if they should be handled...)
lintian debian.deb

# unzip the changelog file, so that svn is not confused because it is missing
gunzip debian/usr/share/doc/hfst-dev/changelog.Debian.gz

# To install the package, execute
# sudo dpkg -i debian.deb

# and to remove
# sudo dpkg -r foo-dev
