check:
	$(top_srcdir)/test/tools/fsmbook-examples/test-example.sh \
	$(srcdir)/DateParser $(builddir)/DateParser \
	../../../../tools/src/

EXTRA_DIST=DateParser.hfst.script DateParser.xfst.script \
	   DateParser.xfst.att

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
