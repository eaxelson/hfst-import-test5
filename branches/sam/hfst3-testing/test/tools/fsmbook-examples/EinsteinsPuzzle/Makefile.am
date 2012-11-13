check:
	$(srcdir)/test.sh $(srcdir) $(builddir) ../../../../tools/src/ # --test-full


EXTRA_DIST=EinsteinsPuzzle.hfst.script test.sh expected_result \
	   EinsteinsPuzzle.xfst.script expected_result_foma

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
