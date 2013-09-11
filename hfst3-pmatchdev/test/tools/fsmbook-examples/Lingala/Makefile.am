check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-foma.sh \
	$(srcdir)/Lingala $(builddir)/Lingala \
	../../../../tools/src/./

EXTRA_DIST=Lingala.hfst.script Lingala.xfst.script

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
