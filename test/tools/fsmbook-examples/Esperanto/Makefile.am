check:
	$(srcdir)/test.sh $(srcdir) $(builddir)

EXTRA_DIST=	EsperantoNouns.hfst.script \
		EsperantoNouns.lexc \
		EsperantoNouns.xfst.att \
		EsperantoAdjectives.hfst.script \
		EsperantoAdjectives.lexc \
		EsperantoAdjectives.xfst.att \
		EsperantoNounsAndAdjectives.hfst.script \
		EsperantoNounsAndAdjectives.lexc \
		EsperantoNounsAndAdjectives.xfst.att \
		EsperantoNounsAndAdjectivesWithTags.hfst.script \
		EsperantoNounsAndAdjectivesWithTags.lexc \
		EsperantoNounsAndAdjectivesWithTags.xfst.att \
		EsperantoNounsAdjectivesAndVerbs.hfst.script \
		EsperantoNounsAdjectivesAndVerbs.lexc \
		EsperantoNounsAdjectivesAndVerbs.xfst.att \
		test.sh

clean-local:
	$(top_srcdir)/test/tools/fsmbook-examples/remove-complement.sh \
	$(EXTRA_DIST) Makefile.am Makefile.in Makefile
