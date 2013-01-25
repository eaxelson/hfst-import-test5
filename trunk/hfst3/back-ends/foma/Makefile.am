AM_CPPFLAGS= -Wno-deprecated -std=c99 # -D_MSC_VER -DWINDOWS 

if WANT_MINGW
AM_CPPFLAGS += -DWINDOWS
endif

noinst_LTLIBRARIES = libfoma.la
libfoma_la_SOURCES = int_stack.c define.c determinize.c apply.c \
        rewrite.c lexcread.c topsort.c flags.c minimize.c reverse.c \
        extract.c sigma.c structures.c constructions.c \
        coaccessible.c io.c utf8.c spelling.c dynarray.c mem.c stringhash.c \
        trie.c lex.lexc.c lex.yy.c regex.tab.c

hfst_foma_LDADD = $(top_builddir)/back-ends/foma/libfoma.la

hfst_foma_LDFLAGS=-lreadline -lz 

if HAVE_TERMCAP
hfst_foma_LDFLAGS += -ltermcap
else
if HAVE_NCURSES
hfst_foma_LDFLAGS += -lncurses
else 
if HAVE_CURSES
hfst_foma_LDFLAGS += -lcurses
endif
endif
endif

if CAN_FOMAWRAP
bin_PROGRAMS=hfst_foma
hfst_foma_SOURCES=foma.c stack.c iface.c lex.interface.c lex.cmatrix.c
endif

EXTRA_DIST=foma.h fomalibconf.h fomalib.h lexc.h regex.tab.h 