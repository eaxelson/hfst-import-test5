check:
	./test.script

EXTRA_DIST=FinnishProsody.hfst.script expected_result.strings test.script

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile