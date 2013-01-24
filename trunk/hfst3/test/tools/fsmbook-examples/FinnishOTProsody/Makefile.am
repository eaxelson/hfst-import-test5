check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-example.sh \
	$(srcdir)/FinnishOTProsody $(builddir)/FinnishOTProsody \
	../../../../tools/src/./ # --test-full

EXTRA_DIST=FinnishOTProsody.hfst.script FinnishOTProsody.xfst.script \
	FinnishOTProsody.xfst.att

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile