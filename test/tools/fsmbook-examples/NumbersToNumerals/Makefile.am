check:
	$(srcdir)/test.sh $(srcdir) $(builddir)

EXTRA_DIST=NumbersToNumerals.hfst.script NumbersToNumerals.xfst.script \
	   NumbersToNumerals.xfst.att test.sh

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
