AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS =  -I$(top_builddir) -I$(top_builddir)/ofst -I$(top_builddir)/sfst
install-exec-hook:
	$(LDCONFIG)
AM_CXXFLAGS = -O3 -Wno-deprecated

lib_LTLIBRARIES = libhfst.la

# we only have one library target but would like a particular includedir
# structure for various headers, so we do this

hfstincludedir = $(includedir)/hfst2
ofstincludedir = $(hfstincludedir)/ofst/fst/lib
sfstincludedir = $(hfstincludedir)/sfst

# automake, specifically ylwrap, sucks horribly
# the following line is (a proposal) for when ylwrap gets fixed to build
# lex-generated headers in the right place.
#
# AM_LFLAGS = -v --outfile=lex.yy.c --header-file=$(<:.ll=.h) \
# 	--prefix=cool_file_dependent_but_not_target_dependent_prefix
#
# Due to this and other problems all lex and yacc stuff will be done by hand
# for the time being. The requisite .ll and .yy -files will be EXTRA_DISTed
# which breaks make distcheck :(
#
# AM_YFLAGS = -d -v \
#	--name_prefix=cool_file_dependent_but_not_target_dependent_prefix
#
# NOTE FOR WHOEVER WOULD LIKE TO FIX THIS: the #define hacks mentioned in eg.
# http://www.gnu.org/software/hello/manual/automake/Yacc-and-Lex.html
# don't work because of our renaming hacks (I think)


EXTRA_DIST = \
	regexp/xre_parse.yy \
	regexp/xre_lex.ll

noinst_HEADERS = \
	string/letter-trie.h \
	regexp/xre.h \
	sfst/optimized_lookup_defs.h \
	sfst/hfst-optimized-lookup-transducer.h \
	ofst/optimized_lookup_defs.h \
	ofst/hwfst-runtime-auxiliary-functions.h \
	ofst/hwfst-optimized-lookup-transducer.h

# First, some things that need to be "built" first. These could be given
# as per-target prerequisites, but
# 1) this way the handmade rules look nicer
# 2) this will ultimately be necessary when/if we get autotools to play nice

BUILT_SOURCES = \
	regexp/xrew.h \
	regexp/xrew_parse.yy \
	regexp/xrew_parse.hh \
	regexp/xrew_lex.ll \
	regexp/xre_parse.hh \
	regexp/xre_lex.h \
	string/w_letter-trie.h \
	string/w_letter-trie.cc \
	string/w_string.cc

# Renaming hackery

regexp/xrew_parse.yy: regexp/xre_parse.yy
	sed -e 's/xre/xrew/g' -e 's/HFST/HWFST/g' $< > $@

regexp/xrew.h: regexp/xre.h
	sed -e 's/xre/xrew/g' -e 's/HFST/HWFST/g' $< > $@

regexp/xrew.cc: regexp/xre.cc
	sed -e 's/xre/xrew/g' -e 's/HFST/HWFST/g' $< > $@

regexp/xrew_api.cc: regexp/xre_api.cc
	sed -e 's/xre/xrew/g' -e 's/HFST/HWFST/g' \
		-e 's/compile_xrew/compile_xre/g' \
		-e 's/xrew_get/xre_get/g' \
		-e 's/xrew_set/xre_set/g' \
		-e 's/xrew_add/xre_add/g' $< > $@

regexp/xrew_lex.ll: regexp/xre_lex.ll
	sed -e 's/xre/xrew/g' -e 's/HFST/HWFST/g' $< > $@

string/w_letter-trie.h: string/letter-trie.h
	sed -e 's/HFST/HWFST/g' -e 's/letter-trie.h/w_letter-trie.h/g' $< > $@

string/w_letter-trie.cc: string/letter-trie.cc
	sed -e 's/HFST/HWFST/g' -e 's/letter-trie.h/w_letter-trie.h/g' $< > $@

string/w_string.cc: string/string.cc
	sed -e 's/HFST/HWFST/g' -e 's/letter-trie.h/w_letter-trie.h/g' $< > $@

# LEX

regexp/xre_lex.cc: regexp/xre_lex.ll
	$(LEX) -v --prefix=xre --outfile=$@ --header-file=regexp/xre_lex.h $<

regexp/xrew_lex.cc: regexp/xrew_lex.ll
	$(LEX) -v --prefix=xrew --outfile=$@ --header-file=regexp/xrew_lex.h $<

regexp/xre_lex.h: regexp/xre_lex.ll
	$(LEX) -v --prefix=xre --outfile=regexp/xre_lex.cc --header-file=$@ $<

regexp/xrew_lex.h: regexp/xrew_lex.ll
	$(LEX) -v --prefix=xrew --outfile=regexp/xrew_lex.cc --header-file=$@ $<

# YACC

regexp/xre_parse.hh: regexp/xre_parse.yy
	$(YACC) -d -v --name-prefix=xre --output=$@ $<

regexp/xrew_parse.hh: regexp/xrew_parse.yy
	$(YACC) -d -v --name-prefix=xrew --output=$@ $<

regexp/xre_parse.cc: regexp/xre_parse.yy
	$(YACC) -d -v --name-prefix=xre --output=$@ $<

regexp/xrew_parse.cc: regexp/xrew_parse.yy
	$(YACC) -d -v --name-prefix=xrew --output=$@ $<

# libtool takes over

libhfst_la_SOURCES = \
	ofst/fst/lib/compat.cc \
	ofst/fst/lib/fst.cc \
	ofst/fst/lib/properties.cc \
	ofst/fst/lib/symbol-table.cc \
	ofst/fst/lib/flags.cc \
	ofst/fst/lib/util.cc \
	ofst/hofst.C \
	ofst/escape.C \
	ofst/harmonize.C \
	ofst/shuffle.C \
	ofst/is-empty-intersection.C \
	ofst/hwfst-key-and-symbol-functions.C \
	ofst/fst_intersecting_compose.C \
	ofst/hwfst-runtime-conversion.C \
	ofst/hwfst-lookup.C \
	ofst/hwfst-tokenize-functions.C \
	ofst/hwfst-read-runtime-transducer.C \
	ofst/hwfst-optimized-lookup-transducer.cc \
	sfst/alphabet.C \
	sfst/basic.C \
	sfst/binSearch.C \
	sfst/compact.C \
	sfst/make-compact.C \
	sfst/determinise.C \
	sfst/utf8.C \
	sfst/hfst-key-and-symbol-functions.C \
	sfst/hsfst.C \
	sfst/operators.C \
	sfst/fst.C \
	sfst/generate.C \
	sfst/fst-lex.C \
	sfst/fst-regression.C \
	sfst/lowmem.C \
	sfst/runtime_conversion.C \
	sfst/read_runtime_transducer.C \
	sfst/lookup.C \
	sfst/tokenize_functions.C \
	sfst/parse_flag_diacritics.C \
	sfst/hfst-optimized-lookup-transducer.cc \
	regexp/xre_parse.cc \
	regexp/xre_lex.cc \
	regexp/xrew_lex.cc \
	regexp/xrew_parse.cc \
	regexp/xre_api.cc \
	regexp/xre.cc \
	regexp/xrew_api.cc \
	regexp/xrew.cc \
	string/letter-trie.cc \
	string/string.cc \
	string/w_letter-trie.cc \
	string/w_string.cc

HFST_HDRS = \
	hfst.h \
	core-extension-layer.h \
	datatypes.h \
	key-layer.h \
	rule-layer.h \
	transducer-layer.h \
	symbol-layer.h \
	range-functions.h \
	symbols_and_symbol_pairs.h \
	hfst_defs_private.h \
	hfst_defs_public.h \
	regexp/regexp.h \
	string/string.h

SFST_HDRS = \
	sfst/alphabet.h sfst/basic.h sfst/compact.h \
	sfst/make-compact.h sfst/fst.h sfst/mem.h sfst/lowmem.h \
	sfst/parse_flag_diacritics.h sfst/utf8.h sfst/lookup.h \
	sfst/fst-lex.h sfst/fst-regression.h sfst/runtime_conversion.h \
        sfst/read_runtime_transducer.h sfst/tokenize_functions.h \
	sfst/runtime_type_defs.h sfst/binSearch.h sfst/transducer-conversion.h

OFST_HDRS = \
	ofst/fst/lib/*.h \
	ofst/escape.h ofst/harmonize.h \
	ofst/fst_intersecting_compose.h \
	ofst/hwfst-tokenize-functions.h \
	ofst/hwfst-lookup-defs.h \
	ofst/is-empty-intersection.h \
	ofst/shuffle.h \
	ofst/hwfst-runtime-conversion.h \
	ofst/hwfst-read-runtime-transducer.h \
	ofst/hwfst-lookup.h \
	ofst/hwfst-runtime-type-defs.h

hfstinclude_HEADERS = $(HFST_HDRS)
sfstinclude_HEADERS = $(SFST_HDRS)
ofstinclude_HEADERS = $(OFST_HDRS)

libhfst_la_LDFLAGS = -version-info 0:0:0
