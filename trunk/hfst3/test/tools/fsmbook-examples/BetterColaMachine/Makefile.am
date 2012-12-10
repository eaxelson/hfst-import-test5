check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-foma.sh \
	$(srcdir)/BetterColaMachine $(builddir)/BetterColaMachine \
	../../../../tools/src/./ # --test-full

EXTRA_DIST=BetterColaMachine.hfst.script BetterColaMachine.xfst.script \
	BetterColaMachine.xfst.att

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile