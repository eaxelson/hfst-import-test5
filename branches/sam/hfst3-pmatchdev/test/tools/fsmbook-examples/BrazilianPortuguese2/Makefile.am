check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-example.sh \
	$(srcdir)/BrazilianPortuguese2 $(builddir)/BrazilianPortuguese2 \
	../../../../tools/src/ # --test-full

EXTRA_DIST=BrazilianPortuguese2.hfst.script BrazilianPortuguese2.xfst.script \
	   BrazilianPortuguese2.xfst.att README

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
