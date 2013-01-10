check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-example.sh \
	$(srcdir)/PlusOrMinus $(builddir)/PlusOrMinus \
	../../../../tools/src/

EXTRA_DIST=PlusOrMinus.hfst.script PlusOrMinus.xfst.script \
	   PlusOrMinus.xfst.att test.script README

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
