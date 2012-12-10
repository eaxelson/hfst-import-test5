
AM_CPPFLAGS= -D_MSC_VER -DWINDOWS -Wno-unused-result -Wno-deprecated

noinst_LTLIBRARIES = libfoma.la
libfoma_la_SOURCES = int_stack.c define.c determinize.c apply.c \
        rewrite.c lexcread.c topsort.c flags.c minimize.c reverse.c \
        extract.c sigma.c structures.c constructions.c \
        coaccessible.c io.c utf8.c spelling.c dynarray.c mem.c stringhash.c \
        trie.c lex.lexc.c lex.yy.c regex.tab.c

EXTRA_DIST=foma.h fomalibconf.h fomalib.h lexc.h regex.tab.h 