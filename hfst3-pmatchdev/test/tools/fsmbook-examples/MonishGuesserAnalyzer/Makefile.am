check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-foma.sh \
	$(srcdir)/MonishGuesserAnalyzer $(builddir)/MonishGuesserAnalyzer \
	../../../../tools/src/  #--test-full

EXTRA_DIST=MonishGuesserAnalyzer.hfst.script MonishGuesserAnalyzer.xfst.script \
	MonishGuesserAnalyzer.xfst.att

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile