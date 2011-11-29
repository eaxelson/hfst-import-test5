#!/bin/sh
#
#  A script for creating the debian package for HFST
#

hfst_tool_dir=../../hfst3/tools/src/.libs/ # the HFST commandline tools
hfst_library_dir=../../hfst3/libhfst/src/.libs/ # the HFST library
hfst_header_dir=../../hfst3/libhfst/src/ # the HFST header files
impl_library_dir=/usr/local/lib/  # the backend implementation libraries


tools=hfst-invert

# Copy the tools
for tool in $tools;
do
    cp $hfst_tool_dir/$tool debian/usr/bin/;
done
strip debian/usr/bin/*

# test..
# mv debian/usr/bin/hfst-invert debian/usr/bin/hfst-invert-foo

# Copy the libraries
cp -P $hfst_library_dir/libhfst.so* debian/usr/lib/
cp -P $impl_library_dir/libfst.so* debian/usr/lib/
cd debian/usr/lib/
rm libfst.so
ln -s -T libfst.so.0.0.0 libfst.so
strip *
chmod 0644 *
cd ../../..

header_files="HfstTransducer.h hfst_apply_schemas.h HfstDataTypes.h \
    HfstSymbolDefs.h implementations/SfstTransducer.h \
    implementations/TropicalWeightTransducer.h \
    implementations/LogWeightTransducer.h implementations/FomaTransducer.h \
    implementations/HfstOlTransducer.h HfstTokenizer.h \
    implementations/ConvertTransducerFormat.h \
    implementations/HfstTransitionGraph.h implementations/HfstTransition.h \
    implementations/HfstTropicalTransducerTransitionData.h \
    implementations/HfstFastTransitionData.h \
    implementations/compose_intersect/ComposeIntersectRulePair.h \
    implementations/compose_intersect/ComposeIntersectLexicon.h \
    implementations/compose_intersect/ComposeIntersectRule.h \
    implementations/compose_intersect/ComposeIntersectFst.h \
    HarmonizeUnknownAndIdentitySymbols.h HfstExceptionDefs.h \
    HfstExtractStrings.h HfstFlagDiacritics.h HfstInputStream.h \
    HfstOutputStream.h parsers/LexcCompiler.h parsers/XreCompiler.h"

# Copy the HFST header files
for header in $header_files;
do
    cp -P $hfst_header_dir/$header debian/usr/include/;
done
chmod 0644 debian/usr/include/*

# Configure the libraries
cp debian/DEBIAN/control debian/control
dpkg-shlibdeps debian/usr/bin/*
rm debian/control

# gzip the changelog file, if needed
if [ -e debian/usr/share/doc/hfst-dev/changelog.Debian ]; then
    gzip --force --best debian/usr/share/doc/hfst-dev/changelog.Debian;
fi

# To make a copy without the .svn files
# tar -c --exclude=.svn debian-test | tar -x -C debian-test-copy
# cd debian-test-copy
# mv debian-test/* .
# rmdir debian-test

# Make the package
fakeroot dpkg-deb --build debian
#fakeroot dpkg-buildpackage

# Test if the package is ok (lintian will give some warnings, 
# see if they should be handled...)
lintian debian.deb

# unzip the changelog file, so that svn is not confused because it is missing
gunzip debian/usr/share/doc/hfst-dev/changelog.Debian.gz

# To install the package, execute
# sudo dpkg -i debian.deb

# and to remove
# sudo dpkg -r foo-dev




