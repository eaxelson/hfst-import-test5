check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-example.sh \
	$(srcdir)/MonishAnalysis $(builddir)/MonishAnalysis \
	../../../../tools/src/  #--test-full

EXTRA_DIST=MonishAnalysis.hfst.script MonishAnalysis.xfst.script \
	MonishAnalysis.xfst.att

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile