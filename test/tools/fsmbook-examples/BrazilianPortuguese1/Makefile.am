check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-example.sh \
	$(srcdir)/BrazilianPortuguese1 $(builddir)/BrazilianPortuguese1 \
	../../../../tools/src/  #--test-full

EXTRA_DIST=BrazilianPortuguese1.hfst.script BrazilianPortuguese1.xfst.script \
	BrazilianPortuguese1.xfst.att

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile