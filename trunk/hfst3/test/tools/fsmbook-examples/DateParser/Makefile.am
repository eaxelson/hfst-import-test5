check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-example.sh \
	$(srcdir)/DateParser $(builddir)/DateParser

EXTRA_DIST=test.sh DateParser.hfst.script DateParser.xfst.script \
	   DateParser.xfst.att test-example.sh remove-complement.sh

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
