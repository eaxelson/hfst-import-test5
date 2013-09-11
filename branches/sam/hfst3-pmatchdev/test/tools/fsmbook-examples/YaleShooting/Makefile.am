check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-foma.sh \
	$(srcdir)/YaleShooting $(builddir)/YaleShooting \
	../../../../tools/src/ # --test-full

EXTRA_DIST=YaleShooting.hfst.script YaleShooting.xfst.script \
	YaleShooting.xfst.att

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile