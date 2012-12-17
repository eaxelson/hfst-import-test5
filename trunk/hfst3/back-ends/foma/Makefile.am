AM_CPPFLAGS= -Wno-deprecated # -D_MSC_VER -DWINDOWS 

noinst_LTLIBRARIES = libfoma.la
libfoma_la_SOURCES = int_stack.c define.c determinize.c apply.c \
        rewrite.c lexcread.c topsort.c flags.c minimize.c reverse.c \
        extract.c sigma.c structures.c constructions.c \
        coaccessible.c io.c utf8.c spelling.c dynarray.c mem.c stringhash.c \
        trie.c lex.lexc.c lex.yy.c regex.tab.c

hfst_foma_LDADD = $(top_builddir)/back-ends/foma/libfoma.la

hfst_foma_LDFLAGS=-lreadline -lz -ltermcap

bin_PROGRAMS=hfst_foma

hfst_foma_SOURCES=foma.c stack.c iface.c lex.interface.c lex.cmatrix.c

EXTRA_DIST=foma.h fomalibconf.h fomalib.h lexc.h regex.tab.h 