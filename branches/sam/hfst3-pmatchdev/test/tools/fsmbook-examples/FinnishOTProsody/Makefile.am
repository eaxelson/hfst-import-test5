check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-foma.sh \
	$(srcdir)/FinnishOTProsody $(builddir)/FinnishOTProsody \
	../../../../tools/src/./

EXTRA_DIST=FinnishOTProsody.hfst.script FinnishOTProsody.xfst.script \
	FinnishOTProsody.xfst.att

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile