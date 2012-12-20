check:
	./test.script

EXTRA_DIST=Palindromes.hfst.script expected_result words.txt

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile