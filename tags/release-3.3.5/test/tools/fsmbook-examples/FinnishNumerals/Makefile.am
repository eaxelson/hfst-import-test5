check:
	$(srcdir)/test.sh $(srcdir) $(builddir) ../../../../tools/src/

EXTRA_DIST=FinnishNumerals.hfst.script test.sh \
	   english_to_finnish_numerals_expected \
	   finnish_to_english_numerals_expected

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
