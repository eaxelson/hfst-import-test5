check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-example.sh \
	$(srcdir)/EnglishNumerals $(builddir)/EnglishNumerals

EXTRA_DIST=EnglishNumerals.hfst.script EnglishNumerals.xfst.script \
	   EnglishNumerals.xfst.att

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
