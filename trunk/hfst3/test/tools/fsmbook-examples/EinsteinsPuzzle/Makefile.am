check:
	$(srcdir)/test.sh $(srcdir) $(builddir) ../../../../tools/src/

EXTRA_DIST=EinsteinsPuzzle.hfst.script test.sh expected_result

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
