#!/bin/sh
#
# A script for generating static binaries for eight tools:
# hfst-xfst, hfst-lexc, hfst-lookup, hfst-optimized-lookup,
# hfst-pmatch, hfst-pmatch2fst, hfst-twolc and hfst-proc.
#

ENVIRONMENT=
SHOW_LINKS=
CONFIGURE_ARGS='--enable-xfst --with-readline --enable-foma-wrapper=no'
if (uname -a | grep 'Linux'); then
    ENVIRONMENT='Linux';
    SHOW_LINKS='ldd';
#   CONFIGURE_ARGS is ok as such 
elif (uname -a | grep 'Darwin'); then
    ENVIRONMENT='Mac';
    SHOW_LINKS='otool -L'
    CONFIGURE_ARGS=$CONFIGURE_ARGS' --disable-dependency-tracking CFLAGS="-arch i386 -arch x86_64" CXXFLAGS="-arch i386 -arch x86_64" LDFLAGS="-arch i386 -arch x86_64"'
else
    echo "Unknown environment"
    exit 1
fi

compile_statically ()
{
    echo "compiling "$1"..."
    make $1 > LOG
    echo "linking "$1" statically..."
    grep 'libtool: link:' LOG | sed 's/libhfst.so/libhfst.a/; s/libtool: link: //' | sh
    if ($SHOW_LINKS .libs/$1 | grep 'libhfst'); then
        echo "static linking of libhfst failed foor tool "$1
        exit 1
    fi
}


autoreconf -i && ./configure $CONFIGURE_ARGS

cd back-ends && make && cd ../libhfst/src && make

cd ../../tools/src/parsers &&
compile_statically hfst-xfst &&
cd ../../../tools/src &&
compile_statically hfst-lexc &&
compile_statically hfst-lookup &&
compile_statically hfst-optimized-lookup &&
compile_statically hfst-pmatch &&
compile_statically hfst-pmatch2fst &&
cd ../../tools/src/hfst-proc &&
compile_statically hfst-apertium-proc &&
cd ../../../tools/src/hfst-twolc/src &&
compile_statically htwolcpre1 &&
compile_statically htwolcpre2 &&
compile_statically htwolcpre3


# Mac: libhfst.dylib
# tweak binary compilation by changing from dynamic libhfst linking to static one



# make "-arch i386 -arch x86_64" for CFLAGS, CXXFLAGS and LDFLAGS and --disable-dependency-tracking. Mac's readline does not find 'rl_abort' in hfst-xfst, so it should be commented out.